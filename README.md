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

# Base FreeRTOS Project (ESP32-ready)

Ngôn ngữ: C. Mục tiêu: base nhẹ cho ứng dụng FreeRTOS (tương thích ESP-IDF/ESP32).

## Mục lục
- Tổng quan
- Build / Flash
- Cấu trúc thư mục
- Cấu hình
- Logging
- Message queue API
- Time helpers
- Ví dụ nhanh

## Tổng quan

Thư mục `main/` chứa lõi của project:
- `main.c` : demo producer/consumer
- `app_config.h` : cấu hình toàn cục
- `core/` : message queue
- `http/`, `mqtt/`, `net/` : scaffold networking (tùy nền tảng)
- `os/` : logging và time helpers

## Build / Flash

Yêu cầu: ESP-IDF hoặc môi trường FreeRTOS tương thích.

Windows PowerShell:

```powershell
idf.py set-target esp32
idf.py build
idf.py -p COMx flash
```

Unix / Bash:

```bash
. $IDF_PATH/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

## Cấu hình (`app_config.h`)

- `APP_CFG_LOG_DEBUG_OFF` : compile-time tắt hoàn toàn log khi định nghĩa.
- `APP_CFG_LOG_LEVEL` : mức log mặc định (nếu không tắt).
- `APP_CFG_MESSAGE_QUEUE_DEPTH`, `APP_CFG_QUEUE_TX_TIMEOUT_MS`, các cấu hình task/stack/prio.

## Logging

Hàm chính:
- `os_log_print(os_log_level_t level, const char *tag, const char *fmt, ...)` — in log với timestamp.
- `os_log_vprint(...)` — biến thể với `va_list`.

Macro tiện lợi (còn tồn tại trong header): `OS_LOGE/W/I/D` nhưng bạn có thể gọi trực tiếp `os_log_print`.

Ví dụ:

```c
os_log_print(OS_LOG_LEVEL_INFO, "MAIN", "Startup ok");
```

## Message queue API

Thiết kế: mỗi message gồm `Sender` (task handle + tên) và con trỏ `void *data` do sender quản lý.

API chính:
- `bool message_queue_init(MessageQueue *q, const char *name, UBaseType_t depth);`
- `void message_queue_deinit(MessageQueue *q);`
- `Sender sender_from_current_task(void);`
- `bool message_queue_send(MessageQueue *q, Sender s, void *data, TickType_t timeout);`
- `bool message_queue_receive(MessageQueue *q, Message *out, TickType_t timeout);`

Lưu ý: queue chỉ copy struct `Message` (metadata + pointer), không copy vùng `data` — caller chịu trách nhiệm cấp phát và giải phóng (ví dụ `pvPortMalloc`/`vPortFree`).

## Time helpers

- `uint64_t os_time_get_cpu_time_us(void);`
- `uint32_t os_time_get_cpu_time_ms(void);`
- `TickType_t os_time_get_tick_count(void);`
- `uint32_t os_time_elapsed_ms(uint64_t start_us);`

## Ví dụ nhanh

Producer gửi payload pointer:

```c
TaskPayload *p = pvPortMalloc(sizeof(*p));
// fill p
message_queue_send(&g_main_queue, sender_from_current_task(), p, pdMS_TO_TICKS(50));
```

Consumer nhận và giải phóng:

```c
Message msg;
if (message_queue_receive(&g_main_queue, &msg, portMAX_DELAY)) {
    TaskPayload *p = (TaskPayload *)msg.data;
    // xử lý
    vPortFree(p);
}
```

---

Tệp này tóm tắt nhanh API và cấu trúc dự án, phù hợp làm điểm khởi đầu cho ứng dụng IoT trên ESP32 hoặc nền tảng FreeRTOS khác.

