# Base FreeRTOS Project (ESP32-ready)

Ngôn ngữ: C — Mục tiêu: mẫu base nhẹ dùng FreeRTOS (đã tối ưu để chạy trên ESP-IDF / ESP32 nhưng có thể port sang board FreeRTOS khác).

## **Mục Lục**
- **Tổng Quan**: cấu trúc dự án
- **Cách Build / Flash**: lệnh nhanh
- **Cấu Trúc Thư Mục**: mô tả file quan trọng
- **Cấu Hình**: `app_config.h`
- **Logging (`os_log`)**: API và cách tắt/bật
- **Message Queue (`message_queue`)**: API và ví dụ
- **Time helpers (`os_time`)**: API
- **Ví dụ sử dụng**: cách tạo producer/consumer
- **Gợi ý mở rộng**

## **Tổng Quan**

Project này cung cấp một base nhỏ gồm:
- `os_log`: logging cơ bản có mức độ (E/W/I/D) và in timestamp
- `os_time`: helper lấy thời gian CPU (microsecond / millisecond) và tick FreeRTOS
- `message_queue`: bus tối giản giữa task dựa trên `QueueHandle_t` của FreeRTOS, truyền `Sender` + `void* data` (caller quản lý vùng nhớ payload)
- `app_config.h`: điểm cấu hình chung (mức log, stack, priority, queue depth,...)

Mục tiêu: bạn chỉ cần include/tuỳ chỉnh `app_config.h` cho board, còn lại dùng API chung.

## **Cách Build / Flash**

Yêu cầu: ESP-IDF đã cài (hoặc môi trường FreeRTOS tương đương). Tham khảo tài liệu ESP-IDF nếu cần thiết.

Windows PowerShell (sau khi đã cấu hình ESP-IDF environment):

```powershell
idf.py set-target esp32
idf.py build
idf.py -p COMx flash
```

Hoặc trên hệ Unix/Bash:

```bash
. $IDF_PATH/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

## **Cấu Trúc Thư Mục (chú ý các file quan trọng)**

- `main/` : mã ứng dụng
  - `main.c` : template producer/consumer demo
  - `app_config.h` : cấu hình chung
  - `core/message_queue.h`, `core/message_queue.c` : message bus
  - `os_log/os_log.h`, `os_log/os_log.c` : logging
  - `os_log/os_time.h`, `os_log/os_time.c` : time helpers

## **Cấu Hình (app_config.h)**

- `APP_CFG_LOG_DEBUG_OFF` : nếu định nghĩa (comment/uncomment), toàn bộ macro `OS_LOG*` sẽ thành no-op (compile-time). Dùng để tắt log cho production.
- `APP_CFG_LOG_LEVEL` : mức log mặc định nếu không tắt hoàn toàn.
- `APP_CFG_MESSAGE_QUEUE_DEPTH` : kích thước queue mặc định.
- `APP_CFG_QUEUE_TX_TIMEOUT_MS` : timeout khi gửi (tx) tính bằng ms.
- `APP_CFG_PRODUCER_PERIOD_MS` / stack / priority: mặc định cho tasks mẫu.

Bạn có thể ghi đè các macro này bằng `-D` trên dòng lệnh build hoặc sửa `app_config.h` theo board.

## **Logging (`os_log`)**

- `OS_LOGE(tag, fmt, ...)` — Error
- `OS_LOGW(tag, fmt, ...)` — Warning
- `OS_LOGI(tag, fmt, ...)` — Info
- `OS_LOGD(tag, fmt, ...)` — Debug

Ví dụ:

```c
OS_LOGI("MAIN", "Startup ok");
OS_LOGD("NET", "Got packet len=%u", len);
```

Tắt log hoàn toàn (compile-time): define `APP_CFG_LOG_DEBUG_OFF` trong `app_config.h` hoặc truyền `-DAPP_CFG_LOG_DEBUG_OFF` cho compiler.

## **Message Queue (`message_queue`)**

Thiết kế: message đơn giản gồm `Sender` (task handle + tên) và `void *data` — dữ liệu do sender cấp phát (malloc) hoặc con trỏ tới vùng tĩnh.

Hàm chính:

- `bool message_queue_init(MessageQueue *queue, const char *name, UBaseType_t depth);`
  - Khởi tạo queue; trả về true nếu thành công.

- `void message_queue_deinit(MessageQueue *queue);`

- `Sender sender_from_current_task(void);`
  - Tạo `Sender` cho task đang chạy.

- `bool message_queue_send(MessageQueue *queue, Sender sender, void *data, TickType_t timeout);`
  - Gửi một `Message` chứa `sender` và `data` (opaque pointer). Hàm sẽ copy struct `Message` vào queue; pointer `data` vẫn trỏ tới vùng của caller — caller phải đảm bảo `data` tồn tại cho tới khi receiver giải phóng.

- `bool message_queue_receive(MessageQueue *queue, Message *out_message, TickType_t timeout);`
  - Nhận message; `out_message->data` là pointer gốc gửi đi. Thông thường receiver sẽ xử lý rồi `vPortFree()` nếu sender dùng `pvPortMalloc()`.

Ví dụ rút gọn gửi/nhận:

```c
// producer
TaskPayload *p = pvPortMalloc(sizeof(*p));
// fill p
message_queue_send(&g_main_queue, sender_from_current_task(), p, pdMS_TO_TICKS(50));

// consumer
Message msg;
if (message_queue_receive(&g_main_queue, &msg, portMAX_DELAY)) {
    TaskPayload *p = (TaskPayload *)msg.data;
    // xử lý
    vPortFree(p);
}
```

Lưu ý:
- Queue chứa copy của struct `Message` (metadata + pointer), nhưng không copy vùng dữ liệu `data` — chính sách này giữ stack usage nhỏ và cho phép truyền pointer. Nếu muốn copy payload, bạn nên gói payload vào `Message` trước khi gửi hoặc điều chỉnh API.

## **Time helpers (`os_time`)**

- `uint64_t os_time_get_cpu_time_us(void);` — microseconds monotonic từ boot (dựa trên `esp_timer_get_time()` cho ESP-IDF).
- `uint32_t os_time_get_cpu_time_ms(void);` — milliseconds.
- `TickType_t os_time_get_tick_count(void);` — FreeRTOS tick count (`xTaskGetTickCount`).
- `uint32_t os_time_elapsed_ms(uint64_t start_us);` — helper tính ms đã trôi.

Ví dụ:

```c
uint64_t t0 = os_time_get_cpu_time_us();
// ...
uint32_t dt = os_time_elapsed_ms(t0);
```

## **Ví dụ sử dụng (tóm tắt)**

- `main.c` trong project là template: khởi tạo một `MessageQueue` global, tạo `producer_task` (mô phỏng QR producer) và `consumer_task` (task chính nhận message, log latency và giải phóng payload).

## **Gợi ý mở rộng**

- Nếu muốn event-driven callback thay vì task-blocking, xem `direct-to-task notifications` hoặc tạo task trung gian nhận queue rồi gọi callback — lưu ý đồng bộ và reentrancy.
- Cân nhắc pool allocator cho payload nếu dùng nhiều message để tránh fragmentation.
- Thêm unit tests (Unity) hoặc host-sim build để test logic não bộ mà không cần phần cứng.

---

Nếu bạn muốn, tôi có thể: 1) thêm README tiếng Anh, 2) tạo template script `build.ps1` cho Windows, hoặc 3) chuyển API `message_queue` sang version copy-payload an toàn (tùy bạn chọn).
