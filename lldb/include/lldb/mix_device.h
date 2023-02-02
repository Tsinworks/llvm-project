#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define READY 0

#define UNPREPARED 1

#define DOWNLOADING 2

#define MOUNTING 3

#define QUERYING 4

/**
 * gputoolsd [DYEmbeddedDaemon handleMessage]
 * GPUTools void -[DYBaseDaemon handleMessage:](DYBaseDaemon *self, SEL a2, id a3)
 * DYBaseStreamTransport _packMessage:error:
 * First packet,       message with plist
 */
#define START 1290

/**
 * enable metal info,  message with plist dict, replyserial == 1, nsarchive
 */
#define SEND_COMMAND 1294

/**
 * Enable capture for the app
 * payload,            message with plist dict
 */
#define SEND_MESSAGE 1286

#define SEND 1292

#define SEND1 1793

#define SEND2 1794

#define SEND3 1537

#define SEND4 1538

#define RECV3 1551

#define RECV4 1550

#define RECV5 1547

#define RECV6 1796

#define RECV7 1538

#define RECV8 1282

#define REPLY 1539

#define REPLY2 1536

#define REPLY1 1541

typedef enum {
  mix_device_abi_x86 = (1 << 0),
  mix_device_abi_x64 = (1 << 1),
  mix_device_abi_arm = (1 << 2),
  mix_device_abi_arm64 = (1 << 3),
  mix_device_abi_loong64 = (1 << 4),
} mix_device_abi;

typedef enum {
  mix_device_event_online,
  mix_device_event_offline,
} mix_device_event;

typedef enum {
  mix_gpu_event_type_none,
  /**
   * PowerVR
   */
  mix_gpu_event_type_pvr_compute_begin,
  mix_gpu_event_type_pvr_compute_end,
  mix_gpu_event_type_pvr_ta_begin,
  mix_gpu_event_type_pvr_ta_end,
  mix_gpu_event_type_pvr_render3_d_begin,
  mix_gpu_event_type_pvr_render3_d_end,
  mix_gpu_event_type_pvr_render2_d_begin,
  mix_gpu_event_type_pvr_render2_d_end,
  mix_gpu_event_type_pvr_rtu_begin,
  mix_gpu_event_type_pvr_rtu_end,
  mix_gpu_event_type_pvr_shg_begin,
  mix_gpu_event_type_pvr_shg_end,
  /**
   * Adreno
   */
  mix_gpu_event_type_adreno_binning_begin,
  mix_gpu_event_type_adreno_binning_end,
  mix_gpu_event_type_adreno_gmem_load_color_begin,
  mix_gpu_event_type_adreno_gmem_load_color_end,
  mix_gpu_event_type_adreno_render_begin,
  mix_gpu_event_type_adreno_render_end,
  mix_gpu_event_type_adreno_gmem_store_color_begin,
  mix_gpu_event_type_adreno_gmem_store_color_end,
  mix_gpu_event_type_adreno_blit_begin,
  mix_gpu_event_type_adreno_blit_end,
  mix_gpu_event_type_adreno_preempt_begin,
  mix_gpu_event_type_adreno_preempt_end,
  mix_gpu_event_type_adreno_gmem_load_depth_stencil_begin,
  mix_gpu_event_type_adreno_gmem_load_depth_stencil_end,
  mix_gpu_event_type_adreno_gmem_store_depth_stencil_begin,
  mix_gpu_event_type_adreno_gmem_store_depth_stencil_end,
  mix_gpu_event_type_adreno_async_compute_begin,
  mix_gpu_event_type_adreno_async_compute_end,
  mix_gpu_event_type_adreno_queue_begin,
  mix_gpu_event_type_adreno_queue_end,
  mix_gpu_event_type_adreno_render_clear_begin,
  mix_gpu_event_type_adreno_render_clear_end,
  mix_gpu_event_type_adreno_workload_begin,
  mix_gpu_event_type_adreno_workload_end,
  mix_gpu_event_type_adreno_dispatch_begin,
  mix_gpu_event_type_adreno_dispatch_end,
  mix_gpu_event_type_adreno_gmem_load_input_begin,
  mix_gpu_event_type_adreno_gmem_load_input_end,
} mix_gpu_event_type;

typedef enum {
  /**
   *< Success
   */
  mix_lldb_connection_status_success,
  /**
   *< End-of-file encountered
   */
  mix_lldb_connection_status_end_of_file,
  /**
   *< Check GetError() for details
   */
  mix_lldb_connection_status_error,
  /**
   *< Request timed out
   */
  mix_lldb_connection_status_timed_out,
  /**
   *< No connection
   */
  mix_lldb_connection_status_no_connection,
  /**
   *< Lost connection while connected to a valid connection
   */
  mix_lldb_connection_status_lost_connection,
  /**
   *< Interrupted read
   */
  mix_lldb_connection_status_interrupted,
} mix_lldb_connection_status;

enum mix_open_mode
#ifdef __cplusplus
  : uint8_t
#endif // __cplusplus
 {
  mix_open_mode_read = 1,
  mix_open_mode_write = 2,
  mix_open_mode_read_write = 3,
  mix_open_mode_exec = 4,
  mix_open_mode_read_exec = 5,
  mix_open_mode_write_exec = 6,
  mix_open_mode_read_write_exec = 7,
};
#ifndef __cplusplus
typedef uint8_t mix_open_mode;
#endif // __cplusplus

typedef enum {
  mix_device_platform_ios = 0,
  mix_device_platform_android = 1,
  mix_device_platform_unsupported = 3,
} mix_device_platform;

typedef struct mix_app_service_t mix_app_service_t;

typedef struct mix_app_service_t mix_app_service_t;

typedef struct mix_device_t mix_device_t;

typedef struct mix_device_tracker_t mix_device_tracker_t;

typedef struct mix_fs_t mix_fs_t;

typedef struct mix_router_t mix_router_t;

typedef struct mix_gpu_tap_agent_t mix_gpu_tap_agent_t;

typedef struct mix_instruments_t mix_instruments_t;

typedef struct mix_logger_t mix_logger_t;

typedef struct mix_screenrec_t mix_screenrec_t;

/**
 * Need to be implemented on driver
 */
typedef struct mix_screenrec_t mix_screenrec_t;

typedef struct mix_screenrec_t mix_screenrec_t;

typedef struct mix_screenshotr_t mix_screenshotr_t;

typedef struct mix_screenshotr_t mix_screenshotr_t;

typedef struct mix_devices_t mix_devices_t;

typedef struct mix_data_t mix_data_t;

typedef void (*ErrorCallback)(const char *error_str, void *usr);

typedef void (*mix_device_event_callback)(mix_device_event, mix_device_t*, void*);

typedef struct {
  /**
   * Read log from device
   */
  uint8_t support_log;
  /**
   * Get file/directory stat
   */
  uint8_t support_stat;
  /**
   * Get performance counters or statistics data
   */
  uint8_t support_instruments;
  uint8_t support_perfetto;
  /**
   * Basic file IO operations
   */
  uint8_t support_file_io;
  uint8_t support_screenshot;
  uint8_t support_screenrecord;
  /**
   * Debug running processes or Launch debuggable process via LLDB or GDB
   */
  uint8_t support_debug;
  uint8_t support_socket_forward;
  uint8_t support_socket_reverse;
  uint8_t support_app_list;
  /**
   * Send notification to device
   */
  uint8_t support_app_notification;
  /**
   * Support shell commands
   */
  uint8_t support_shell;
  /**
   * Install/Uninstall app package
   */
  uint8_t support_app_install;
  uint8_t support_image_mount;
} mix_features;

typedef void (*mix_videorec_start)(size_t total, void*);

typedef size_t (*mix_writer_fn_t)(const uint8_t*, size_t, void*);

typedef void (*mix_videorec_end)(size_t total, void*);

typedef struct {
  mix_videorec_start on_start;
  mix_writer_fn_t on_data;
  mix_videorec_end on_end;
  void *usr;
} mix_videorec_callbacks;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t bit_rate;
  uint32_t seconds;
} mix_videorec_settings;

/**
 * Log callback return whether to contine logging
 * # Arguments
 * * `chars`: Log content in chars
 * * `num_chars`: Number of chars in this callback
 * * `usr_data`: c-FFI user data
 */
typedef int (*mix_log_callback)(mix_device_platform platform, const char *chars, int num_chars, void *usr_data);

typedef void (*mix_app_install_status_callback)(const char*, void*);

typedef struct {
  unsigned char with_icon;
  unsigned char with_version;
} mix_app_list_option;

typedef void (*mix_app_list_on_begin)(size_t app_count, void*);

typedef int32_t (*mix_app_list_should_get_icon)(const char *bundle_id, void*);

typedef void (*mix_app_list_on_iter)(const char *bundle_id, const char *display_name, const char *version, const uint8_t *icon_png_data, size_t icon_length, void*);

typedef void (*mix_app_list_on_end)(void*);

typedef struct {
  mix_app_list_on_begin begin;
  mix_app_list_should_get_icon should_get_icon;
  mix_app_list_on_iter callback;
  mix_app_list_on_end end;
  void *usr_data;
} mix_app_list_callbacks;

typedef struct {
  uint64_t dev;
  uint64_t ino;
  uint32_t mode;
  uint32_t nlink;
  uint32_t uid;
  uint32_t gid;
  uint64_t size;
  int64_t atime;
  int64_t mtime;
  int64_t ctime;
} mix_stat;

typedef size_t (*mix_reader_fn_t)(uint8_t*, size_t, void*);

typedef struct {
  const uint8_t *name;
  const uint8_t *desc;
  uint8_t id;
  int8_t is_percents;
} mix_gpu_counter_desc;

typedef struct {
  const uint8_t *name;
  uint32_t id;
} mix_gpu_event_desc;

typedef struct {
  /**
   * For example, Adreno 640, A12X, M1 Max, Mali-G78 MP20, etc..
   */
  const char *device_name;
  uint64_t min_collection_interval;
  size_t num_counters;
  const mix_gpu_counter_desc *counter_descs;
  size_t num_events;
  const mix_gpu_event_desc *event_descs;
  /**
   * Support adreno render stages or PVR gpu events
   */
  uint32_t support_render_stages;
  /**
   * Only AppleGPU supports (Adreno vulkan maybe)
   */
  uint32_t support_shader_timeline;
  /**
   * Surface packets
   */
  uint32_t support_surface_packets;
  /**
   * Select counters
   */
  uint32_t support_metric_selection;
} mix_gpu_device_info;

/**
 * Set gpu tracing PID, counter sample rate and interval
 */
typedef struct {
  /**
   * Sample interval in milliseconds
   */
  double sample_interval;
  double sample_rate;
  /**
   * If tracing PID not provided or equals `-1`,
   * you have to retry `start_trace` and `stop_strace` (**gpu_tap_agent**)
   * many times to get gpu events returned.
   */
  int32_t tracing_pid;
} mix_gpu_config;

/**
 * Gpu counter samples callback
 */
typedef void (*mix_gpu_counter_callback)(size_t num_samples, const uint8_t *counter_nums, const uint64_t *timestamps, const uint8_t *counter_ids, const float *counter_values, void *usr_data);

typedef struct {
  /**
   * In microseconds
   */
  double timestamp;
  mix_gpu_event_type ty;
} mix_gpu_event;

/**
 * GpuEvents callback, not shader function callback
 */
typedef void (*mix_gpu_event_callback)(size_t num_events, const mix_gpu_event *events_buffer, void *usr_data);

typedef struct {
  uint64_t start;
  uint64_t end;
  uint16_t num_bins;
  uint16_t surf_width;
  uint16_t surf_height;
  uint16_t bin_width;
  uint16_t bin_height;
} mix_gpu_surface;

typedef void (*mix_gpu_surfaces_callback)(size_t num_surface, const mix_gpu_surface *surfaces, void *usr_data);

typedef void (*mix_gpu_new_counter_callback)(size_t num_counters, const mix_gpu_counter_desc *counter_descs, void *usr_data);

typedef void (*mix_gpu_new_event_callback)(size_t num_events, const mix_gpu_event_desc *event_descs, void *usr_data);

/**
 * Use this structure to receive perf-data from device
 */
typedef struct {
  mix_gpu_counter_callback counters;
  mix_gpu_event_callback events;
  mix_gpu_surfaces_callback surfaces;
  mix_gpu_new_counter_callback new_counter;
  mix_gpu_new_event_callback new_event;
} mix_gpu_tap_callbacks;

typedef void (*mix_prepare_status_callback)(uint32_t stage, uint32_t progress, void*);

typedef void (*mix_progress_fn_t)(uint32_t, void*);

typedef void (*mix_lldb_connection_status_callback)(mix_lldb_connection_status status, const char *error, void *usr_data);

typedef struct mix_lldb_debugger mix_lldb_debugger;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void mix_free_string(char *s);

/**
 * Access data blob memory
 */
uint8_t *mix_data_pointer(mix_data_t *data);

/**
 * Get size of binary data blob
 */
size_t mix_data_length(mix_data_t *data);

void mix_free_data(mix_data_t *data);

const char *mix_version(void);

/**
 * List all mobile devices
 *
 * **Don't** call it frequently!
 * # Arguments
 * * `devices` - Return connected devices
 * # Example
 * ```ignore
 * mix_devices list = nullptr;
 * mix_list_devices(&list, false);
 * uint32_t count = mix_devices_count(list);
 * auto device = mix_device_at(0);
 * mix_release_devices(list);
 * ```
 */
void mix_list_devices(mix_devices_t **devices);

/**
 * url starts with ios:// or android://
 */
void mix_connect_device(const char *url, mix_device_t **dev_ptr, ErrorCallback err_fn, void *usr);

size_t mix_devices_count(mix_devices_t *devices);

mix_device_t *mix_device_at(mix_devices_t *devices, size_t index);

void mix_release_device(mix_device_t *device);

mix_device_tracker_t *mix_create_device_tracker(void);

void mix_release_device_tracker(mix_device_tracker_t *devtracker);

/**
 * Start listen device events [[Async]]
 */
void mix_device_tracker_start_listen(mix_device_tracker_t *devtracker);

void mix_device_tracker_stop_listen(mix_device_tracker_t *devtracker);

mix_device_t *mix_device_clone_to_heap(mix_device_t *device);

/**
 * * Callback will be called in another thread, keep it thread-safe!
 * * Device is allocated on the stack, if you want to make it persistent, you need to copy it to heap by calling `device_clone_to_heap`.
 */
void mix_device_tracker_add_subscriber(mix_device_tracker_t *devtracker,
                                       void *usr_data,
                                       mix_device_event_callback callback);

void mix_device_tracker_remove_subscriber(mix_device_tracker_t *devtracker, void *usr_data);

void mix_release_devices(mix_devices_t *devices);

char *mix_device_get_uid(mix_device_t *device);

char *mix_device_get_os_ver(mix_device_t *device);

char *mix_device_get_brand(mix_device_t *device);

mix_device_platform mix_device_get_platform(mix_device_t *device);

char *mix_device_get_name(mix_device_t *device);

/**
 * For android, read from prop "ro.product.model"
 */
char *mix_device_get_model(mix_device_t *device);

bool mix_device_support_abi(mix_device_t *device, mix_device_abi abi);

void mix_device_get_features(mix_device_t *device, mix_features *features, size_t _size);

mix_screenshotr_t *mix_create_screenshotr(mix_device_t *device);

void mix_release_screenshotr(mix_screenshotr_t *screenshotr);

/**
 * Create screenshot on the device. (thread-safe)
 * # Arguments
 * * `data` - Out **PNG** data blob, need to be freed by calling `free_data`
 */
void mix_screenshotr_take(mix_screenshotr_t *screenshotr, mix_data_t **data);

/**
 * Create screen recorder
 */
mix_screenrec_t *mix_create_screenrec(mix_device_t *device, const mix_videorec_callbacks *cbs);

/**
 * Destroy screen recorder
 */
void mix_release_screenrec(mix_screenrec_t *screenrec);

/**
 * Screen recorder start recording
 */
void mix_screenrec_start(mix_screenrec_t *screenrec, const mix_videorec_settings *settings);

/**
 * Screen recorder stop recording
 */
void mix_screenrec_stop(mix_screenrec_t *screenrec);

/**
 * Create device logger (support filters)
 */
mix_logger_t *mix_create_logger(mix_device_t *device);

void mix_release_logger(mix_logger_t *logger);

/**
 * Start logger with tags(optional)
 * # Arguments
 * - `tags`: seperated by `|`, case sensitive
 */
void mix_start_logger(mix_logger_t *logger,
                      mix_log_callback func,
                      const char *tags,
                      void *usr_data);

void mix_stop_logger(mix_logger_t *logger);

mix_router_t *mix_create_router(mix_device_t *device);

void mix_release_router(mix_router_t *forwarder);

/**
 * Add device socket forwarding
 * # Arguments
 * - `local_addr` : tcp:{host_port}
 * - `remote_addr` : For android, it could be localabstract:{name} or tcp:{port}. For iOS, it could be {port}
 * - `reverse` : Only supports android
 */
void mix_add_forward(mix_router_t *forwarder,
                     const char *local,
                     const char *remote,
                     bool reverse);

/**
 * Remove device socket forwarding
 */
void mix_remove_forward(mix_router_t *forwarder,
                        const char *local,
                        const char *remote,
                        bool reverse);

mix_app_service_t *mix_create_app_service(mix_device_t *device);

void mix_release_app_service(mix_app_service_t *app_service);

/**
 * Upload and install application package to the target device
 * # Arguments
 * - `path`: Application package on **host** path
 */
void mix_apps_install(mix_app_service_t *app_service,
                      const char *path,
                      mix_app_install_status_callback status_proc,
                      void *usr_data);

/**
 * Uninstall app on device
 * # Arguments
 * - `app_id`: Application identifier
 */
void mix_apps_uninstall(mix_app_service_t *app_service, const char *app_id);

/**
 * List installed application package on the target device
 * # Arguments
 * - `option`: list options
 */
void mix_apps_list(mix_app_service_t *app_service,
                   const mix_app_list_option *option,
                   size_t _size,
                   mix_app_list_callbacks callback_obj);

/**
 * Create file transmission session
 * # Argument
 * - `bundle_id` : if is null, it will access rootfs; If not null, it should follow the pattern "{app_id}:\[document|container\]", for example, "com.apple.Keynote:container".
 */
mix_fs_t *mix_create_fs(mix_device_t *device,
                        const char *bundle_id);

void mix_release_fs(mix_fs_t *file_session);

void mix_fs_stat(mix_fs_t *file_session, const char *path, mix_stat *stat, size_t _size);

void mix_fs_mkdir(mix_fs_t *file_session, const char *path);

void mix_fs_rmdir(mix_fs_t *file_session, const char *path);

void mix_fs_rmfile(mix_fs_t *file_session, const char *path);

void mix_fs_listdir(mix_fs_t *file_session, const char *path);

void mix_fs_pushfile(mix_fs_t *file_session,
                     const char *path,
                     mix_open_mode mode,
                     size_t size,
                     mix_reader_fn_t reader,
                     void *user_data);

void mix_fs_pullfile(mix_fs_t *file_session,
                     const char *path,
                     mix_writer_fn_t writer,
                     void *user_data);

/**
 * Create instruments client to acquire performance data
 */
mix_instruments_t *mix_create_instruments(mix_device_t *device);

void mix_release_instruments(mix_instruments_t *instru);

void mix_instruments_start(mix_instruments_t *instru);

void mix_instruments_stop(mix_instruments_t *instru);

/**
 * Should be called after started
 */
mix_gpu_tap_agent_t *mix_instruments_create_gpu_tap_agent(mix_instruments_t *instru);

void mix_instruments_release_gpu_tap_agent(mix_gpu_tap_agent_t *gpu_tap);

/**
 * Should be called after started
 */
void mix_instruments_gpu_tap_get_gpu_info(mix_gpu_tap_agent_t *gpu_tap,
                                          mix_gpu_device_info *info,
                                          size_t _size_info);

void mix_instruments_gpu_tap_set_config(mix_gpu_tap_agent_t *gpu_tap,
                                        const mix_gpu_config *config,
                                        size_t _size_of_config);

/**
 * Register profiler event callbacks, include counters array and events array
 */
void mix_instruments_gpu_tap_register_callbacks(mix_gpu_tap_agent_t *gpu_tap,
                                                mix_gpu_counter_callback counters,
                                                mix_gpu_event_callback events,
                                                void *data);

/**
 * # Arguments:
 * - `_size_of_callbacks`: For compatibilities
 */
void mix_instruments_gpu_tap_register_callbacks2(mix_gpu_tap_agent_t *gpu_tap,
                                                 const mix_gpu_tap_callbacks *callbacks,
                                                 void *data,
                                                 size_t _size_of_callbacks);

void mix_instruments_gpu_tap_unregister_callbacks(mix_gpu_tap_agent_t *gpu_tap, void *data);

void mix_instruments_gpu_tap_start_capture(mix_gpu_tap_agent_t *gpu_tap);

void mix_instruments_gpu_tap_stop_capture(mix_gpu_tap_agent_t *gpu_tap);

/**
 * Execute shell command on the target device
 */
void mix_shell(mix_device_t *device, const char *cmd_line);

/**
 * Prepare device utilities (blocking mode)
 * ## Details
 * * Download and mount developer disk image for profiling and debugging
 */
void mix_device_prepare_tools(mix_device_t *device, mix_prepare_status_callback pfn, void *usr);

/**
 * Query current prepare state
 */
uint32_t mix_device_get_prepare_state(mix_device_t *device);

/**
 * Mount disk image to remote device
 */
void mix_mount_disk_image(mix_device_t *device,
                          const char *path,
                          mix_progress_fn_t progress,
                          void *user_data);

bool mix_device_has_developer_image(mix_device_t *device);

void mix_device_resolve_executable(mix_device_t *device,
                                   const char *host_app,
                                   void (*result)(const char*, const char*, const char*, void*),
                                   ErrorCallback status,
                                   void *usr);

mix_lldb_debugger *mix_device_create_lldb_connection(mix_device_t *device,
                                                     const char *port,
                                                     mix_lldb_connection_status_callback status,
                                                     void *usr);

void mix_lldb_connection_free(mix_lldb_debugger *debugger);

void mix_lldb_connection_disconnect(mix_lldb_debugger *debugger,
                                    mix_lldb_connection_status_callback status,
                                    void *usr);

bool mix_lldb_connection_is_connected(mix_lldb_debugger *debugger);

size_t mix_lldb_connection_read(mix_lldb_debugger *debugger,
                                void *dst,
                                size_t len,
                                uint64_t to,
                                mix_lldb_connection_status_callback status,
                                void *usr);

/**
 * size_t Write(const void *dst, size_t dst_len, lldb::ConnectionStatus &status, Status *error_ptr)
 */
size_t mix_lldb_connection_write(mix_lldb_debugger *debugger,
                                 const void *src,
                                 size_t len,
                                 mix_lldb_connection_status_callback status,
                                 void *usr);

/**
 * bool InterruptRead()
 */
bool mix_lldb_connection_interrupt_read(mix_lldb_debugger *debugger);

int mix_lldb_connection_native_handle(mix_lldb_debugger *debugger);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
