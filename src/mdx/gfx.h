#pragma once

namespace mdx
{
    class Window;

    class Gfx
    {
    public:
        Gfx() = default;
        ~Gfx() = default;

        void Init(Window* wnd, bool enableDebugLayer);
        void Close();
        void Frame();

        bool IsDebugLayerEnabled() const;

    private:
        Gfx(const Gfx&) = delete;
        Gfx(Gfx&&) = delete;
        Gfx& operator=(const Gfx&) = delete;
        Gfx& operator=(Gfx&&) = delete;

        void SelectBestAdapter();
        void CreateDeviceAndQueues();
        void CreateSwapChain();
        void CreateRenderTargets();
        void CreateCommandLists();
        void Present();

    private:
        Window* m_wnd = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Debug3> m_d3dDebug;
        Microsoft::WRL::ComPtr<ID3D12DebugDevice2> m_d3dDebugDevice;
        Microsoft::WRL::ComPtr<ID3D12InfoQueue> m_d3dInfoQueue;
        Microsoft::WRL::ComPtr<ID3D12Device6> m_d3dDevice;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dGfxQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dCopyQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dComputeQueue;
        
        Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_dxgiSwapChain;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3dDescriptorHeapRTV;       
        Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dBackBuffers[2];
        D3D12_CPU_DESCRIPTOR_HANDLE m_d3dBackBuffersRTV[2];

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3dCommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> m_d3dCommandList;
        Microsoft::WRL::ComPtr<ID3D12Fence1> m_d3dPresentFence;

        HANDLE m_presentFenceEvent = INVALID_HANDLE_VALUE;
        UINT m_descriptorSizeRTV = 0;

        bool m_isDebugLayerEnabled = false;
        bool m_isTearingSupported = false;
        bool m_isVSyncEnabled = false;
        bool m_isFullscreenExclusive = false;
    };
}
