#include "platform.h"
#include "core.h"
#include "gfx.h"
#include "app.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

namespace mdx
{
    static void DXGI_VALIDATE(HRESULT result)
    {
        if (SUCCEEDED(result))
            return;

        //TODO create DXGI exception with error code and message
        throw Exception("DXGI Error %d", result);
    }

    static void D3D_VALIDATE(HRESULT result)
    {
        if (SUCCEEDED(result))
            return;

        //TODO create D3D exception with error code and message
        throw Exception("D3D Error %d", result);
    }

    void Gfx::Init(Window* wnd, bool enableDebugLayer)
    {
        m_wnd = wnd;
        m_isDebugLayerEnabled = enableDebugLayer;

        UINT flags = 0;
        if (m_isDebugLayerEnabled)
            flags |= DXGI_CREATE_FACTORY_DEBUG;
        
        DXGI_VALIDATE(::CreateDXGIFactory2(flags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

        BOOL tearingSupport = FALSE;
        DXGI_VALIDATE(m_dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearingSupport, sizeof(tearingSupport)));
        m_isTearingSupported = tearingSupport != 0;

        SelectBestAdapter();
        CreateDeviceAndQueues();
        CreateSwapChain();

        //handle fullscreen<->windowed transitions ourself
        DXGI_VALIDATE(m_dxgiFactory->MakeWindowAssociation(m_wnd->GetHandle(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN | DXGI_MWA_NO_WINDOW_CHANGES));

        CreateRenderTargets();
        CreateCommandLists();
    }

    void Gfx::Close()
    {
        //TODO wait for any in-flight resources to not be used by the GPU anymore before releasing them

        m_wnd = nullptr;
        m_dxgiSwapChain.Reset();
        m_d3dGfxQueue.Reset();
        m_d3dCopyQueue.Reset();
        m_d3dInfoQueue.Reset();
        m_d3dDebugDevice.Reset();
        m_d3dDebug.Reset();
        m_d3dDevice.Reset();
        m_dxgiAdapter.Reset();
        m_dxgiFactory.Reset();
    }

    void Gfx::Present()
    {
        UINT presentFlags = 0;
        if (!m_isFullscreenExclusive && m_isTearingSupported)
            presentFlags |= DXGI_PRESENT_ALLOW_TEARING;

        DXGI_PRESENT_PARAMETERS pp = {};
        m_dxgiSwapChain->Present1(m_isVSyncEnabled ? 1u : 0u, presentFlags, &pp);
    }

    void Gfx::Frame()
    {
        D3D_VALIDATE(m_d3dCommandAllocatorsGfx[m_frameIndex]->Reset());
        D3D_VALIDATE(m_d3dCommandListGfx->Reset(m_d3dCommandAllocatorsGfx[m_frameIndex].Get(), nullptr));

        //set root signature
        //TODO

        //set viewport and scissor rect
        const SIZE clientSize = m_wnd->GetClientSize();
        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(clientSize.cx), static_cast<float>(clientSize.cy));
        CD3DX12_RECT     scissorRect(0, 0, clientSize.cx, clientSize.cy);
        m_d3dCommandListGfx->RSSetViewports(1, &viewport);
        m_d3dCommandListGfx->RSSetScissorRects(1, &scissorRect);

        //set render target
        auto rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3dBackBuffers[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_d3dCommandListGfx->ResourceBarrier(1, &rtBarrier);
        m_d3dCommandListGfx->OMSetRenderTargets(1, &m_d3dBackBuffersRTV[m_frameIndex], FALSE, nullptr);
        
        //clear render target
        const FLOAT clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        m_d3dCommandListGfx->ClearRenderTargetView(m_d3dBackBuffersRTV[m_frameIndex], clearColor, 0, nullptr);

        //finish rendering this frame and prepare to present
        rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3dBackBuffers[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_d3dCommandListGfx->ResourceBarrier(1, &rtBarrier);
        D3D_VALIDATE(m_d3dCommandListGfx->Close());

        ID3D12CommandList* commandLists[] = { m_d3dCommandListGfx.Get() };
        m_d3dGfxQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        Present();

        //signal that we're done with this frame
        D3D_VALIDATE(m_d3dPresentFence->SetEventOnCompletion(m_currentFrame, m_presentFenceEvent));
        D3D_VALIDATE(m_d3dGfxQueue->Signal(m_d3dPresentFence.Get(), m_currentFrame));

        m_currentFrame++;
        m_frameIndex = m_currentFrame & 0x1;

        //still not done rendering with previous frame's buffer and command allocator? wait for those to be available
        if (m_d3dPresentFence->GetCompletedValue() < (m_currentFrame - 1))
            ::WaitForSingleObject(m_presentFenceEvent, INFINITE);
    }

    void Gfx::SelectBestAdapter()
    {
        DXGI_VALIDATE(m_dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(m_dxgiAdapter.ReleaseAndGetAddressOf())));
    }

    void Gfx::CreateDeviceAndQueues()
    {
        //enable debug layer before creating device
        if (m_isDebugLayerEnabled)
        {
            D3D_VALIDATE(D3D12GetDebugInterface(IID_PPV_ARGS(m_d3dDebug.ReleaseAndGetAddressOf())));
            m_d3dDebug->EnableDebugLayer();
        }

        D3D_VALIDATE(D3D12CreateDevice(m_dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf())));

        //setup debug device
        if (m_isDebugLayerEnabled)
        {
            if (SUCCEEDED(m_d3dDevice.As(&m_d3dDebugDevice)))
            {
                //TODO if needed
            }

            if (SUCCEEDED(m_d3dDevice.As(&m_d3dInfoQueue)))
            {
                m_d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
                m_d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
                m_d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

                m_d3dInfoQueue->SetMuteDebugOutput(FALSE);

                D3D12_MESSAGE_SEVERITY denySeverities[] =
                {
                    D3D12_MESSAGE_SEVERITY_INFO,
                };

                D3D12_INFO_QUEUE_FILTER filter = {};
                filter.DenyList.NumSeverities = _countof(denySeverities);
                filter.DenyList.pSeverityList = denySeverities;
                D3D_VALIDATE(m_d3dInfoQueue->PushStorageFilter(&filter));
            }
        }

        //create graphics queue
        D3D12_COMMAND_QUEUE_DESC gqd = {};
        gqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        gqd.NodeMask = 0;
        gqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        gqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        D3D_VALIDATE(m_d3dDevice->CreateCommandQueue(&gqd, IID_PPV_ARGS(m_d3dGfxQueue.ReleaseAndGetAddressOf())));

        //create present fence & event
        D3D_VALIDATE(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_d3dPresentFence.ReleaseAndGetAddressOf())));
        m_presentFenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);

        //create copy/upload queue
        D3D12_COMMAND_QUEUE_DESC uqd = {};
        uqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        uqd.NodeMask = 0;
        uqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        uqd.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        D3D_VALIDATE(m_d3dDevice->CreateCommandQueue(&uqd, IID_PPV_ARGS(m_d3dCopyQueue.ReleaseAndGetAddressOf())));

        //create compute queue? TBD
    }

    void Gfx::CreateSwapChain()
    {
        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.BufferCount = _countof(m_d3dBackBuffers);
        sd.BufferUsage = DXGI_CPU_ACCESS_NONE;
        sd.Flags = m_isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : static_cast<UINT>(0);
        sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.Height = 0;
        sd.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Width = 0;

        DXGI_VALIDATE(m_dxgiFactory->CreateSwapChainForHwnd(m_d3dGfxQueue.Get(), m_wnd->GetHandle(), &sd, nullptr, nullptr, m_dxgiSwapChain.ReleaseAndGetAddressOf()));
    }

    void Gfx::CreateRenderTargets()
    {
        //create RTV descriptor heap if needed
        if (!m_d3dDescriptorHeapRTV)
        {
            D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
            dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            dhd.NumDescriptors = _countof(m_d3dBackBuffersRTV);
            dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            dhd.NodeMask = 0;
            D3D_VALIDATE(m_d3dDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(m_d3dDescriptorHeapRTV.ReleaseAndGetAddressOf())));
            
            m_descriptorSizeRTV = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        //retrieve the back buffers
        DXGI_SWAP_CHAIN_DESC1 sd = {};
        DXGI_VALIDATE(m_dxgiSwapChain->GetDesc1(&sd));
        for (UINT i = 0; i < sd.BufferCount; i++)
            DXGI_VALIDATE(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(m_d3dBackBuffers[i].ReleaseAndGetAddressOf())));

        //create the RTVs
        CD3DX12_CPU_DESCRIPTOR_HANDLE dhStart(m_d3dDescriptorHeapRTV->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < sd.BufferCount; i++)
        {
            m_d3dBackBuffersRTV[i] = dhStart.Offset(static_cast<INT>(i),m_descriptorSizeRTV);
            m_d3dDevice->CreateRenderTargetView(m_d3dBackBuffers[i].Get(), nullptr, m_d3dBackBuffersRTV[i]);
        }
    }

    void Gfx::CreateCommandLists()
    {
        //create command allocators, since we're double-buffered, one command allocator per frame
        for(int i = 0; i < _countof(m_d3dCommandAllocatorsGfx); i++)
            D3D_VALIDATE(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_d3dCommandAllocatorsGfx[i].ReleaseAndGetAddressOf())));

        //create command lists, only need one for now, can re-use between frames
        D3D_VALIDATE(m_d3dDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(m_d3dCommandListGfx.ReleaseAndGetAddressOf())));
    }
}