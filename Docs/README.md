# Rancage Render Engine

Rancage Render Engine adalah proyek render engine berbasis DirectX 12 (C++) dengan editor berbasis C# WPF. Tujuan utama adalah membangun engine yang mampu merender model 3D kompleks seperti Sponza dengan performa tinggi dan fitur modern (PBR, deferred rendering, dll.). Proyek ini dikembangkan secara bertahap, dengan setiap langkah didokumentasikan untuk memudahkan pengembangan dan kolaborasi.

## Perjalanan Pengembangan

Berikut adalah ringkasan versi pengembangan dari awal hingga saat ini:

### Versi 0.0.0.1: Inisialisasi Window dan Debug Layer

- **Tujuan**: Membuat window dasar menggunakan Win32 API dan menyiapkan debug layer Direct3D 12.
- **Fitur**:
  - Window dibuat dengan resolusi 1280x720, judul "Rancage Render Engine".
  - Menangani pesan dasar (`WM_CREATE`, `WM_PAINT`, `WM_DESTROY`) melalui `WindowProc`.
  - Inisialisasi logger sederhana untuk mencatat ke file (`Logs/engine.log`).
  - Setup debug layer untuk validasi Direct3D 12 (awalnya aktif).
- **Masalah**:
  - Crash di message loop window, exit code 1.
  - Logger file (`wofstream`) sering putus di `[INFO]`, menyebabkan crash.
  - Debug layer bikin aplikasi berat dan kadang crash di beberapa konfigurasi.
- **Solusi**:
  - Tambah logging pesan window (`WM_NCCREATE`, `WM_KEYDOWN`, dll.) untuk debug.
  - Nonaktifkan file output logger, ganti ke console-only (`std::wcout`) sementara.
  - Matikan debug layer untuk stabilitas (`DX12 debug layer disabled`).

### Versi 0.0.0.2: Inisialisasi Device

- **Tujuan**: Menambahkan Direct3D 12 device untuk komunikasi dengan GPU.
- **Fitur**:
  - Membuat DXGI factory untuk enumerasi adapter.
  - Memilih adapter dengan VRAM tertinggi (NVIDIA GeForce RTX 3060, 5996 MB).
  - Membuat D3D12 device dengan feature level 11.0.
  - Tambah NVIDIA Optimus hint (`NvOptimusEnablement`) untuk pastikan RTX 3060 terpilih.
- **Masalah**:
  - Crash window masih terjadi karena cleanup tidak terpanggil.
  - Visual Studio kadang ikut crash karena unhandled exception.
- **Solusi**:
  - Tambah try-catch di `Application::Run` dan `main` untuk nangkap exception.
  - Perkuat `Window::~Window` dengan `DestroyWindow` dan `UnregisterClass`.
  - Redirect console ke `Logs\engine.txt` via `Engine.exe > Logs\engine.txt` untuk debug.

### Versi 0.0.0.3: Command Queue, SwapChain, dan Stabilisasi

- **Tujuan**: Menyiapkan command queue dan SwapChain untuk rendering, sambil pastikan exit code 0.
- **Fitur**:
  - Tambah `Command` class untuk command queue, allocator, dan list (`D3D12_COMMAND_LIST_TYPE_DIRECT`).
  - Tambah `SwapChain` class untuk double buffering (format `DXGI_FORMAT_R8G8B8A8_UNORM`, flip discard).
  - Integrasi `Command` dan `SwapChain` ke `Device`, menggunakan window handle.
  - Window stabil: support Escape key, timeout 10 detik di `Application::Run`.
  - Logger console-only dengan redirect ke file, log lengkap (`SwapChain created`, `Application stopped`, `Window destroyed`).
  - Tambah try-catch di `Window::ProcessMessages` untuk handle `PeekMessage`/`DispatchMessage` error.
- **Masalah**:
  - Upaya balik ke file logger (`wofstream`) gagal, crash lagi (exit code 1).
  - Console-only logger stabil tapi kurang ideal untuk high volume log ke depan.
- **Solusi**:
  - Tetap gunakan console-only logger untuk stabilitas.
  - Redirect console output ke `Logs\engine.txt` sebagai workaround.
  - Rencanakan AsyncLogger untuk skalabilitas di masa depan.

### Versi 0.0.0.4: Descriptor Heaps dan Render Target Views

- **Tujuan**: Siapkan render target untuk output visual.
- **Fitur** (Direct3D 12 Steps):
  - **Create Descriptor Heaps**: Bikin RTV heap untuk 2 buffer (`D3D12_DESCRIPTOR_HEAP_TYPE_RTV`).
  - **Get Buffers**: Ambil back buffer dari `SwapChain`.
  - **Create Render Target Views**: Setup RTV untuk double buffering.
  - Logger console-only tetap stabil, log tambahan (`RTV descriptor heap created`, `Render target view created`).
- **Status**:
  - Exit code 0, siap untuk clear screen atau rendering sederhana.
  - Window, Device, Command, SwapChain, RTV jalan mulus.

## Struktur File

Berikut adalah file utama di proyek dan fungsinya:

### Core

- **include/Core/Application.h, src/Core/Application.cpp**:
  - Kelas utama untuk inisialisasi dan menjalankan aplikasi.
  - Mengatur `Window`, `Device`, dan loop utama (`Run`).
  - Fitur: Timeout 10 detik, try-catch untuk exit code 0, NVIDIA Optimus hint untuk RTX 3060.
- **include/Core/Window.h, src/Core/Window.cpp**:
  - Mengelola window Win32 (1280x720, "Rancage Render Engine").
  - Menangani pesan (`WM_PAINT`, `WM_KEYDOWN` untuk Escape, `WM_DESTROY`).
  - Fitur: Try-catch di `ProcessMessages`, logging pesan detail (`Processing message: 0x%04X`).
- **include/Core/Debug.h, src/Core/Debug.cpp**:
  - Mengelola debug layer Direct3D 12.
  - Saat ini dinonaktifkan untuk hindari crash.

### Graphics

- **include/Graphics/Device.h, src/Graphics/Device.cpp**:
  - Mengelola Direct3D 12 device dan DXGI factory.
  - Memilih RTX 3060 berdasarkan VRAM tertinggi.
  - Menginisialisasi `Command` dan `SwapChain`.
- **include/Graphics/Command.h, src/Graphics/Command.cpp**:
  - Mengelola command queue, allocator, dan list untuk eksekusi GPU.
  - Tipe: `D3D12_COMMAND_LIST_TYPE_DIRECT`.
- **include/Graphics/SwapChain.h, src/Graphics/SwapChain.cpp**:
  - Mengelola SwapChain untuk double buffering.
  - Format: `DXGI_FORMAT_R8G8B8A8_UNORM`, 2 buffer, swap effect flip discard.

### Utils

- **include/Utils/Logger.h, src/Core/Logger.cpp**:
  - Sistem logging berbasis console (`std::wcout`).
  - Thread-safe dengan mutex.
  - Level: `[INFO]`, `[WARNING]`, `[ERROR]`.
  - Workaround: Redirect ke `Logs\engine.txt` via `Engine.exe > Logs\engine.txt`.
  - Catatan: File output (`wofstream`) menyebabkan crash, ditunda sampai AsyncLogger.

## Status Saat Ini

- **Versi**: 0.0.0.4
- **Fitur**:
  - Window stabil (Escape, timeout 10 detik).
  - Device pake RTX 3060 (5996 MB VRAM).
  - Command queue, allocator, list, SwapChain, RTV siap.
  - Logger console-only, redirect ke `engine.txt`.
- **Exit Code**: 0 (Debug configuration).
- **Direct3D 12 Steps Tercover**:
  - Enable Debug Functionality (dimatikan).
  - Create DXGI Factory, Query Adapters, Create Device.
  - Create Command Queue, Allocator, Lists.
  - Create Swap Chain, Descriptor Heaps, Render Target Views.
- **Rencana**:
  - Clear screen ke warna (misal biru) untuk tes visual.
  - Create Depth Buffer, Depth Stencil View (DSV).
  - Create Fence untuk sync GPU-CPU.
  - AsyncLogger sebelum Sponza untuk high volume log.
  - Pipeline, vertex buffer, dll., untuk render Sponza.

## Cara Menjalankan

1. Build di Visual Studio (Debug, x64):

   ```bash
   Ctrl+Shift+B
   ```

2. Jalankan tanpa debug:

   ```bash
   Ctrl+F5
   ```

   Atau via Command Prompt untuk simpan log:

   ```bash
   cd D:\Projects\Visual Studio\Rancage Render Engine\x64\Debug
   Engine.exe > Logs\engine.txt
   ```

3. Cek `Logs\engine.txt` untuk debug.

## Catatan

- Console-only logger cukup untuk tahap ini (setup rendering awal).
- File output akan diganti ke AsyncLogger untuk skalabilitas saat rendering Sponza.
- Struktur file di `EngineCore/include` dan `EngineCore/src` dirancang modular untuk ekspansi.
- Fokus berikutnya: Render target untuk lihat output visual di window.