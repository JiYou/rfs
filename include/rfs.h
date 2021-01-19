#ifndef _RFS_H_
#define _RFS_H_

#include <stdint.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

struct my_fs {
    
  int mkfs(uint64_t fs_id, uint64_t user_id, );
  int mount();
  int maybe_verify_layout(const bluefs_layout_t &layout) const;
  void umount();
  int prepare_new_device(int id, const bluefs_layout_t &layout);

  int log_dump();

  void collect_metadata(map<string, string> *pm, unsigned skip_bdev_id);
  void get_devices(set<string> *ls);
  uint64_t get_alloc_size(int id) { return alloc_size[id]; }
  int fsck();

  int device_migrate_to_new(CephContext *cct, const set<int> &devs_source,
                            int dev_target, const bluefs_layout_t &layout);
  int device_migrate_to_existing(CephContext *cct, const set<int> &devs_source,
                                 int dev_target, const bluefs_layout_t &layout);

  uint64_t get_used();
  uint64_t get_total(unsigned id);
  uint64_t get_free(unsigned id);
  void get_usage(vector<pair<uint64_t, uint64_t>> *usage); // [<free,total> ...]
  void dump_perf_counters(Formatter *f);

  void dump_block_extents(ostream &out);

  /// get current extents that we own for given block device
  int get_block_extents(unsigned id, interval_set<uint64_t> *extents);

  int open_for_write(const string &dir, const string &file, FileWriter **h,
                     bool overwrite);

  int open_for_read(const string &dir, const string &file, FileReader **h,
                    bool random = false);

  void close_writer(FileWriter *h) {
    std::lock_guard l(lock);
    _close_writer(h);
  }

  int rename(const string &old_dir, const string &old_file,
             const string &new_dir, const string &new_file);

  int readdir(const string &dirname, vector<string> *ls);

  int unlink(const string &dirname, const string &filename);
  int mkdir(const string &dirname);
  int rmdir(const string &dirname);
  bool wal_is_rotational();

  bool dir_exists(const string &dirname);
  int stat(const string &dirname, const string &filename, uint64_t *size,
           utime_t *mtime);

  int lock_file(const string &dirname, const string &filename, FileLock **p);
  int unlock_file(FileLock *l);

  void compact_log();

  /// sync any uncommitted state to disk
  void sync_metadata();

  void set_slow_device_expander(BlueFSDeviceExpander *a) {
    slow_dev_expander = a;
  }
  void set_volume_selector(BlueFSVolumeSelector *s) { vselector.reset(s); }
  void dump_volume_selector(ostream &sout) { vselector->dump(sout); }
  void get_vselector_paths(const std::string &base,
                           BlueFSVolumeSelector::paths &res) const {
    return vselector->get_paths(base, res);
  }

  int add_block_device(unsigned bdev, const string &path, bool trim,
                       bool shared_with_bluestore = false);
  bool bdev_support_label(unsigned id);
  uint64_t get_block_device_size(unsigned bdev);

  /// gift more block space
  void add_block_extent(unsigned bdev, uint64_t offset, uint64_t len) {
    std::unique_lock l(lock);
    _add_block_extent(bdev, offset, len);
    int r = _flush_and_sync_log(l);
    ceph_assert(r == 0);
  }

  /// reclaim block space
  int reclaim_blocks(unsigned bdev, uint64_t want, PExtentVector *extents);

  // handler for discard event
  void handle_discard(unsigned dev, interval_set<uint64_t> &to_release);

  void flush(FileWriter *h) {
    std::lock_guard l(lock);
    _flush(h, false);
  }
  void flush_range(FileWriter *h, uint64_t offset, uint64_t length) {
    std::lock_guard l(lock);
    _flush_range(h, offset, length);
  }
  int fsync(FileWriter *h) {
    std::unique_lock l(lock);
    return _fsync(h, l);
  }
  int read(FileReader *h, FileReaderBuffer *buf, uint64_t offset, size_t len,
           bufferlist *outbl, char *out) {
    // no need to hold the global lock here; we only touch h and
    // h->file, and read vs write or delete is already protected (via
    // atomics and asserts).
    return _read(h, buf, offset, len, outbl, out);
  }
  int read_random(FileReader *h, uint64_t offset, size_t len, char *out) {
    // no need to hold the global lock here; we only touch h and
    // h->file, and read vs write or delete is already protected (via
    // atomics and asserts).
    return _read_random(h, offset, len, out);
  }
  void invalidate_cache(FileRef f, uint64_t offset, uint64_t len) {
    std::lock_guard l(lock);
    _invalidate_cache(f, offset, len);
  }
  int preallocate(FileRef f, uint64_t offset, uint64_t len) {
    std::lock_guard l(lock);
    return _preallocate(f, offset, len);
  }
  int truncate(FileWriter *h, uint64_t offset) {
    std::lock_guard l(lock);
    return _truncate(h, offset);
  }

  /// test purpose methods
  void debug_inject_duplicate_gift(unsigned bdev, uint64_t offset,
                                   uint64_t len);
  const PerfCounters *get_perf_counters() const { return logger; }
};

#endif
