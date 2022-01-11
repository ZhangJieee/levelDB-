// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/table_builder.h"

#include <cassert>

#include "leveldb/comparator.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/options.h"
#include "table/block_builder.h"
#include "table/filter_block.h"
#include "table/format.h"
#include "util/coding.h"
#include "util/crc32c.h"

namespace leveldb {

//TableBuilder内部使用的结构，记录当前的一些状态等
struct TableBuilder::Rep {
  Rep(const Options& opt, WritableFile* f)
      : options(opt),
        index_block_options(opt),
        file(f),
        offset(0),
        data_block(&options),
        index_block(&index_block_options),
        num_entries(0),
        closed(false),
        filter_block(opt.filter_policy == nullptr
                         ? nullptr
                         : new FilterBlockBuilder(opt.filter_policy)),
        pending_index_entry(false) {
    index_block_options.block_restart_interval = 1;    //Index Block中每个restart块只有一个record，查找方便
  }

  Options options;
  Options index_block_options;
  WritableFile* file;          //对应的sst文件
  uint64_t offset;
  Status status;
  BlockBuilder data_block;     //data block
  BlockBuilder index_block;    //index block
  std::string last_key;        //block中添加的最后一个key，一方面用于key是否排序的判断,另一方面当写入data block时，记录index Block中索引项(last_key+offset+size)
  int64_t num_entries;         //sst文件中的key数量
  bool closed;  // Either Finish() or Abandon() has been called.
  FilterBlockBuilder* filter_block;


	// Add下一Block的第一个key/value时，才根据这个key构造一个FindShortSuccessor，
	// 写入Index Block中的一个entry（max_key+offset+size），是为了能够找到
	// 一个更短的分割2个Block的key，从而减少存储容量；
  // We do not emit the index entry for a block until we have seen the
  // first key for the next data block.  This allows us to use shorter
  // keys in the index block.  For example, consider a block boundary
  // between the keys "the quick brown fox" and "the who".  We can use
  // "the r" as the key for the index block entry since it is >= all
  // entries in the first block and < all entries in subsequent
  // blocks.
  //
  // Invariant: r->pending_index_entry is true only if data_block is empty.
  bool pending_index_entry;       // 标识是否刚写入一个Data Block，控制在Index Block中添加一项索引信息(last_key+offset+size)
  BlockHandle pending_handle;  // Handle to add to index block

  std::string compressed_output;   //数据压缩
};

TableBuilder::TableBuilder(const Options& options, WritableFile* file)
    : rep_(new Rep(options, file)) {
  if (rep_->filter_block != nullptr) {
    rep_->filter_block->StartBlock(0);
  }
}

TableBuilder::~TableBuilder() {
  assert(rep_->closed);  // Catch errors where caller forgot to call Finish()
  delete rep_->filter_block;
  delete rep_;
}

//改变配置属性
Status TableBuilder::ChangeOptions(const Options& options) {
  // Note: if more fields are added to Options, update
  // this function to catch changes that should not be allowed to
  // change in the middle of building a Table.
  //使用过程中，不能改变comparator，否则，顺序不能保证有序
  if (options.comparator != rep_->options.comparator) {
    return Status::InvalidArgument("changing comparator while building table");
  }

  // Note that any live BlockBuilders point to rep_->options and therefore
  // will automatically pick up the updated options.
  rep_->options = options;
  rep_->index_block_options = options;
  rep_->index_block_options.block_restart_interval = 1;
  return Status::OK();
}

// sst添加一个key/value
void TableBuilder::Add(const Slice& key, const Slice& value) {
  Rep* r = rep_;
  assert(!r->closed);
  if (!ok()) return;
  if (r->num_entries > 0) {
    assert(r->options.comparator->Compare(key, Slice(r->last_key)) > 0);
  }

  //旧的block结束，新的block开始
  if (r->pending_index_entry) {
    assert(r->data_block.empty());

    // Add下一Data Block的第一个key/value时，才根据这个key构造一个FindShortSuccessor，
	  // 写入Index Block中的一个entry（max_key+offset+size），目的在于为了能够找到
	  // 一个更短的分割2个Block的key，从而减少存储容量
	  // 只有Finish中是根据最后一个Block的最后一个key构造的
    r->options.comparator->FindShortestSeparator(&r->last_key, key);   // 这里生成max_key
    std::string handle_encoding;
    r->pending_handle.EncodeTo(&handle_encoding);

    // 注意，这里每次写满一个block，同时记录一个index block,最后finish阶段在全部写入file
    r->index_block.Add(r->last_key, Slice(handle_encoding));           // Index Block数据，添加刚写入.sst文件中的Data Block索引项(max_key、offset、size)
    r->pending_index_entry = false;
  }

  //添加block filter key
  if (r->filter_block != nullptr) {
    r->filter_block->AddKey(key);
  }

  //当前block中的最大key
  r->last_key.assign(key.data(), key.size());

  //key数量
  r->num_entries++;

  //添加到data block中
  r->data_block.Add(key, value);

  const size_t estimated_block_size = r->data_block.CurrentSizeEstimate();

  //DataBlock容量大于设置的block size，则写入文件
  if (estimated_block_size >= r->options.block_size) {
    Flush();
  }
}

// data block同步到文件中
void TableBuilder::Flush() {
  Rep* r = rep_;
  assert(!r->closed);
  if (!ok()) return;
  if (r->data_block.empty()) return;
  assert(!r->pending_index_entry);

  //向文件写入一个Block(数据及type和CRC)，并设置index Block项(index Block在sst文件完毕阶段写入)
  //在pending_handle中记录Index Block中对应此Block的索引项,记录offset和size信息
  WriteBlock(&r->data_block, &r->pending_handle);
  if (ok()) {

    //设置标志: Add/Finish时，在Index Block中记录一项索引信息
    r->pending_index_entry = true;
    r->status = r->file->Flush();
  }
  if (r->filter_block != nullptr) {
    r->filter_block->StartBlock(r->offset);
  }
}

//向文件写入一个Block(数据及type和CRC)，并设置index Block项
void TableBuilder::WriteBlock(BlockBuilder* block, BlockHandle* handle) {
  // File format contains a sequence of blocks where each block has:
  //    block_data: uint8[n]
  //    type: uint8
  //    crc: uint32
  assert(ok());
  Rep* r = rep_;

  //添加restart信息，返回Block数据的起始位置
  Slice raw = block->Finish();

  Slice block_contents;

  //写入配置，压缩形式
  CompressionType type = r->options.compression;
  // TODO(postrelease): Support more compression options: zlib?
  switch (type) {
    case kNoCompression:
      block_contents = raw;
      break;

    case kSnappyCompression: {                         //进行Snappy压缩
      std::string* compressed = &r->compressed_output;
      if (port::Snappy_Compress(raw.data(), raw.size(), compressed) &&
          compressed->size() < raw.size() - (raw.size() / 8u)) {
        block_contents = *compressed;
      } else {
        // Snappy not supported, or compressed less than 12.5%, so just
        // store uncompressed form
        block_contents = raw;
        type = kNoCompression;
      }
      break;
    }
  }
  WriteRawBlock(block_contents, type, handle);
  r->compressed_output.clear();
  block->Reset();
}

void TableBuilder::WriteRawBlock(const Slice& block_contents,
                                 CompressionType type, BlockHandle* handle) {
  Rep* r = rep_;

  //记录Block的索引信息-offset
  handle->set_offset(r->offset);

  //记录Block的索引信息-size
  handle->set_size(block_contents.size());

  //Block数据写入文件
  r->status = r->file->Append(block_contents);

  //写入成功，再写入type,crc
  if (r->status.ok()) {
    char trailer[kBlockTrailerSize];
    trailer[0] = type;
    uint32_t crc = crc32c::Value(block_contents.data(), block_contents.size());
    crc = crc32c::Extend(crc, trailer, 1);  // Extend crc to cover block type
    EncodeFixed32(trailer + 1, crc32c::Mask(crc));
    r->status = r->file->Append(Slice(trailer, kBlockTrailerSize));

    //一个block完整写入，记录offset
    if (r->status.ok()) {
      r->offset += block_contents.size() + kBlockTrailerSize;
    }
  }
}

Status TableBuilder::status() const { return rep_->status; }

//.sst数据构造完毕，写入文件
Status TableBuilder::Finish() {
  Rep* r = rep_;
  Flush();
  assert(!r->closed);
  r->closed = true;

  BlockHandle filter_block_handle, metaindex_block_handle, index_block_handle;

  // Write filter block
  if (ok() && r->filter_block != nullptr) {
    WriteRawBlock(r->filter_block->Finish(), kNoCompression,
                  &filter_block_handle);
  }

  // Write metaindex block
  // 补充filter block
  if (ok()) {
    BlockBuilder meta_index_block(&r->options);
    if (r->filter_block != nullptr) {
      // Add mapping from "filter.Name" to location of filter data
      std::string key = "filter.";
      key.append(r->options.filter_policy->Name());
      std::string handle_encoding;
      filter_block_handle.EncodeTo(&handle_encoding);
      meta_index_block.Add(key, handle_encoding);
    }

    // TODO(postrelease): Add stats and other meta blocks
    //写入Meta Index Block
    WriteBlock(&meta_index_block, &metaindex_block_handle);
  }

  // Write index block
  // 补充index block
  if (ok()) {
    if (r->pending_index_entry) {

      //sst最后一个block会以last_kay作为构造
      r->options.comparator->FindShortSuccessor(&r->last_key);
      std::string handle_encoding;
      r->pending_handle.EncodeTo(&handle_encoding);

      //在Index Block中增加一个索引信息
      r->index_block.Add(r->last_key, Slice(handle_encoding));
      r->pending_index_entry = false;
    }
    WriteBlock(&r->index_block, &index_block_handle);
  }

  // Write footer
  // 补充footer block
  if (ok()) {
    Footer footer;
    footer.set_metaindex_handle(metaindex_block_handle);
    footer.set_index_handle(index_block_handle);

    // magic number
    std::string footer_encoding;
    footer.EncodeTo(&footer_encoding);

    //写入footer
    r->status = r->file->Append(footer_encoding);
    if (r->status.ok()) {
      r->offset += footer_encoding.size();
    }
  }
  return r->status;
}

void TableBuilder::Abandon() {
  Rep* r = rep_;
  assert(!r->closed);
  r->closed = true;
}

uint64_t TableBuilder::NumEntries() const { return rep_->num_entries; }

uint64_t TableBuilder::FileSize() const { return rep_->offset; }

}  // namespace leveldb
