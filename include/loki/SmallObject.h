#ifndef LOKI_SMALLOBJECT_H
#define LOKI_SMALLOBJECT_H

#include "loki/loki.h"
#include "loki/Singleton.h"
#include "loki/ThreadModel.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

#define DEFAULT_CHUNK_SIZE 4096
#define MAX_SMALL_OBJECT_SIZE 64
#define DEFAULT_THREADING SingleThreaded

LOKI_NS_BEGIN

struct Chunk {
  void Init(size_t blockSize, unsigned char blocks) {
    pData_ = new unsigned char[blockSize * blocks];
    firstAvailableBlock_ = 0;
    blocksAvailable_ = blocks;
    unsigned char i = 0;
    unsigned char* p = pData_;
    for (; i != blocks; p += blockSize) {
     *p = ++i;
    }
  }

  void* Allocate(size_t blockSize) {
   if (!blocksAvailable_) return 0;
   unsigned char* pResult =
    pData_ + (firstAvailableBlock_ * blockSize);
    // Update firstAvailableBlock_ to point to the next block
    firstAvailableBlock_ = *pResult;
    --blocksAvailable_;
    return pResult;
  }

  void Deallocate(void* p, std::size_t blockSize) {
    // std::cout << "Chunk Deallocate" << std::endl;
    assert(p >= pData_);
    unsigned char* toRelease = static_cast<unsigned char*>(p); // Alignment check
    assert((toRelease - pData_) % blockSize == 0);
    *toRelease = firstAvailableBlock_;
    firstAvailableBlock_ = static_cast<unsigned char>(
           (toRelease - pData_) / blockSize);
     // Truncation check
     assert(firstAvailableBlock_ ==
        (toRelease - pData_) / blockSize);
     ++blocksAvailable_;
  }

  unsigned char* pData_;
  unsigned char firstAvailableBlock_, blocksAvailable_;
};

class FixedAllocator {
 public:
  FixedAllocator(size_t blockSize, unsigned char numBlocks)
      : blockSize_(blockSize), numBlocks_(numBlocks),
        allocChunk_(nullptr), deallocChunk_(nullptr),
        deallocChunkIdx_(0) {
  }

  ~FixedAllocator() {
    for (const auto& chunk : chunks_) {
      delete[] chunk.pData_;
    }
  }

  void* Allocate() {
    if (allocChunk_ == nullptr ||
        allocChunk_->blocksAvailable_ == 0) {
      // No available memory in this chunk
      // Try to find one
      Chunks::iterator i = chunks_.begin();
      for (;; ++i) {
        if (i == chunks_.end()) {
      // All filled up-add a new chunk
          chunks_.reserve(chunks_.size()+1);
          Chunk newChunk;
          newChunk.Init(blockSize_, numBlocks_);
          chunks_.push_back(newChunk);
          allocChunk_ = &chunks_.back();
          deallocChunk_ = &chunks_.back();
          deallocChunkIdx_ = chunks_.size() - 1;

          break;
        }  // end if
        if (i->blocksAvailable_ > 0) {
          // Found a chunk
          allocChunk_ = &*i;
          break;
        }  // end if
      }  // end for
    }  // end if allocChunk_
    assert(allocChunk_ != nullptr);
    assert(allocChunk_->blocksAvailable_ > 0);
    return allocChunk_->Allocate(blockSize_);
  }

  bool WithinChunk(void* p, const Chunk& chunk) {
    size_t chunkSize = static_cast<size_t>(numBlocks_) * blockSize_;
    // std::cout << "Chunk Size: " << chunkSize << std::endl;
    if (p < chunk.pData_) {
      return false;
    }
    // std::cout << "WithinChunk Out" << std::endl;
    if (p >= chunk.pData_ + chunkSize) {
      return false;
    }
    // std::cout << "WithinChunk Out" << std::endl;
    return true;
  }

  void Deallocate(void* p) {
    if (WithinChunk(p, *deallocChunk_)) {
      deallocChunk_->Deallocate(p, blockSize_);
      return;
    }

    // Search chunks for deallocation from deallocChunk_ in both
    // forward and backward direction
    bool toHead = false;
    bool toEnd = false;
    auto deallocChunkFwdIt = chunks_.begin() + deallocChunkIdx_;
    assert(deallocChunkFwdIt != chunks_.end());
    size_t deallocChunkFwdIdx = deallocChunkIdx_;
    auto deallocChunkBwdIt = chunks_.begin() + deallocChunkIdx_;
    assert(deallocChunkBwdIt != chunks_.end());
    size_t deallocChunkBwdIdx = deallocChunkIdx_;

    // std::cout << "Enter Dealloc Loop" << std::endl;
    while (true) {
      // std::cout << "In Dealloc Loop" << std::endl;
      if (!toHead) {
        // std::cout << "A" << std::endl;
        if (WithinChunk(p,*deallocChunkBwdIt)) {
          deallocChunkBwdIt->Deallocate(p, blockSize_);
          deallocChunk_ = &*deallocChunkBwdIt;
          deallocChunkIdx_ = deallocChunkBwdIdx;
          break;
        } else if (deallocChunkBwdIt != chunks_.begin()){
          --deallocChunkBwdIt;
          --deallocChunkBwdIdx;
        } else {
          toHead = true;
        } // end if
      }  // end if toHead

      if (!toEnd) {
        if (WithinChunk(p,*deallocChunkFwdIt)) {
          deallocChunkFwdIt->Deallocate(p, blockSize_);
          deallocChunk_ = &*deallocChunkFwdIt;
          deallocChunkIdx_ = deallocChunkFwdIdx;
          break;
        }
        ++deallocChunkFwdIt;
        ++deallocChunkFwdIdx;
        if (deallocChunkFwdIt == chunks_.end()){
          toEnd = true;
        } // end if
      }  // end if toEnd
    }  // end while
    assert(!(toHead && toEnd));
    // If dealloacChunk is entirely empty and
    // there exist multiple chunks
    if (chunks_.size() > 1 && deallocChunk_->blocksAvailable_ == numBlocks_) {
      // If the existing last chunk is already empty, erase it
      auto lastChunkIt = chunks_.end() - 1;
      if (lastChunkIt->blocksAvailable_ == numBlocks_) {
        delete[] lastChunkIt->pData_;
        chunks_.erase(lastChunkIt);
      }
      //   swap to the last chunk
      std::swap(*deallocChunk_, chunks_.back());
      deallocChunk_ = &chunks_.back();
      deallocChunkIdx_ = chunks_.size() - 1;
    }  // end if
  }

 private:
   size_t blockSize_;
   unsigned char numBlocks_;
   using Chunks = std::vector<Chunk>;
   Chunks chunks_;
   Chunk* allocChunk_;
   Chunk* deallocChunk_;
   size_t deallocChunkIdx_;
};

class SmallObjAllocator {
 public:
  SmallObjAllocator(size_t chunkSize, size_t maxObjectSize) :
    chunkSize_(chunkSize), maxObjectSize_(maxObjectSize) {}

  void* Allocate(size_t numBytes) {
    if (numBytes > maxObjectSize_) { return malloc(numBytes); }

    auto fixAllocerIt = pool_.find(numBytes);
    if (fixAllocerIt == pool_.end()) {
      size_t blockSize = numBytes;

      unsigned char numBlock = static_cast<unsigned char>(
                                 std::min(chunkSize_ / blockSize, size_t(0xFF)));

      FixedAllocator allocer(blockSize, numBlock);
      pool_.emplace(std::make_pair(blockSize, allocer));
      fixAllocerIt = pool_.find(blockSize);
    }

    assert(fixAllocerIt != pool_.end());
    return fixAllocerIt->second.Allocate();
  }

  void Deallocate(void* p, size_t size) {
    if (size > maxObjectSize_) {free (p);}
    auto fixAllocerIt = pool_.find(size);
    assert(fixAllocerIt != pool_.end());
    return fixAllocerIt->second.Deallocate(p);
  }

 private:
  size_t chunkSize_;
  size_t maxObjectSize_;
  std::map<size_t, FixedAllocator> pool_;
};

using MyAlloc = Singleton<SmallObjAllocator, DEFAULT_THREADING>;

template<size_t chunkSize = DEFAULT_CHUNK_SIZE,
         size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE>
class SmallObject {
 public:
  static void* operator new(size_t size) {
    return MyAlloc::MakeSingleton(chunkSize, maxSmallObjectSize)->Allocate(size);
  }
  static void operator delete(void* p, size_t size) {
    MyAlloc::MakeSingleton(chunkSize, maxSmallObjectSize)->Deallocate(p, size);
  }
  virtual ~SmallObject() {}
};

LOKI_NS_END

#endif  // LOKI_SMALLOBJECT_H
