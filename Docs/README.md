# Rancage Render Engine

Rancage Render Engine adalah proyek render engine berbasis DirectX 12 (C++) dengan editor berbasis C# WPF. Tujuan utama adalah membangun engine yang mampu merender model 3D kompleks seperti Sponza dengan performa tinggi dan fitur modern (PBR, deferred rendering, dll.). Proyek ini dikembangkan secara bertahap, dengan setiap langkah didokumentasikan untuk memudahkan pengembangan dan kolaborasi.

## Versi 0.0.0.2: Setup DX12 Device

**Tanggal**: 15 April 2025  
**Deskripsi**: Menambahkan inisialisasi DXGI factory, query adapters, dan pembuatan D3D12 device. Kelas `Application` mengatur inisialisasi debug layer dan device.

### Fitur
- Membuat DXGI factory dengan debug support.
- Memilih hardware adapter yang support D3D12 (skip software adapter).
- Membuat D3D12 device dengan feature level 11.0.
- Logging untuk setiap langkah dan error handling.

### Catatan
- Device siap untuk langkah berikutnya (swap chain, command queue).
- Error handling memastikan hanya hardware adapter yang dipilih.
- Device sekarang memilih adapter dengan dedicated VRAM tertinggi untuk performa optimal (misalnya, NVIDIA GPU pada laptop dual GPU).
- Mendukung fallback ke integrated GPU kalau discrete GPU gagal.

## Versi 0.0.0.1: Setup Debug Layer dan Logger

**Tanggal**: 15 April 2025  
**Deskripsi**: Langkah pertama ini fokus pada inisialisasi debug layer DirectX 12 dan sistem logging untuk mendukung pengembangan engine. Debug layer membantu mendeteksi error DX12, sementara logger mencatat informasi penting dengan timestamp dan thread safety, siap untuk debugging scene kompleks seperti Sponza.

### Fitur
- **Debug Layer DX12**:
  - Mengaktifkan debug layer untuk validasi API DX12.
  - Mengaktifkan GPU-based validation dan synchronized command queue validation.
  - Mengatur D3D12 info queue untuk menangkap pesan error dan corruption dengan breakpoint.
- **Logger**:
  - Mendukung level log (`Info`, `Warning`, `Error`).
  - Menulis ke console dan file (`logs/engine.log`) dengan timestamp.
  - Thread-safe menggunakan mutex untuk penggunaan multi-thread.
  - Mendukung Unicode untuk path file dan format string variadic untuk fleksibilitas.

### Struktur Proyek

- `Debug.h/cpp`: Namespace `Debug` untuk inisialisasi debug layer dan info queue.
- `Logger.h/cpp`: Namespace `Logger` untuk sistem logging dengan timestamp dan thread safety.
- `Application.h/cpp`: Entry point sementara untuk memanggil `Debug::Initialize` dan `Logger::Init`.
- `logs/engine.log`: File log output (dibuat otomatis saat program berjalan).

### Prasyarat
- Visual Studio 2022 (atau versi yang support C++17 dan DX12).
- Windows SDK (versi 10.0.19041.0 atau lebih baru untuk DX12).
- Git (untuk push ke GitHub).

### Cara Build dan Run
1. **Buka Proyek**:
   - Buka `RancageRenderEngine.sln` di Visual Studio.
2. **Konfigurasi Proyek**:
   - Pastikan konfigurasi Debug (x64).
   - Project properties:
     - **C/C++ → General → Additional Include Directories**: `$(ProjectDir)include`
     - **Linker → Input → Additional Dependencies**: `d3d12.lib;dxgi.lib`
     - **C/C++ → Language → C++ Language Standard**: `ISO C++17 Standard (/std:c++17)`
3. **Build**:
   - Klik **Build → Build Solution** (Ctrl+Shift+B).
4. **Run**:
   - Tekan F5 untuk jalankan.
   - Program akan membuat file `logs/engine.log` dengan output seperti:
   
### Output
- File `logs/engine.log` berisi log inisialisasi debug layer.
- Console menampilkan log yang sama dengan timestamp.
- Program keluar dengan kode 0 (sukses).

### Catatan
- Debug layer hanya aktif di build Debug (`#ifdef _DEBUG`).
- Logger mendukung Unicode untuk path file, siap untuk asset loading (misalnya, model Sponza).
- Langkah ini adalah fondasi untuk debugging DX12, penting untuk deteksi error saat rendering model kompleks.

### Langkah Berikutnya
- Implementasi `Device` untuk langkah 2-4 flow DX12 (DXGI factory, adapters, device).
- Menyusun kelas `Application` untuk mengatur inisialisasi engine.

### Kontribusi
- Fork repositori ini di GitHub.
- Buat branch untuk tiap langkah (`step-0.1`, `step-0.2`, dst.).
- Submit pull request untuk saran atau perbaikan.

### Lisensi
Belum ditentukan (akan diupdate di langkah berikutnya).