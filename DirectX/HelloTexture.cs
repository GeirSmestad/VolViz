using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SharpDX.DXGI;
using System.Threading;

namespace VolViz.DirectX
{
    using Data;
    using Logic;
    using SharpDX;
    using SharpDX.Direct3D12;
    using SharpDX.Windows;
    using System.IO;
    using System.Reflection;
    using System.Runtime.InteropServices;

    class HelloTexture : IDisposable
    {

        private Volume volume;
        private VolumeRenderer renderer;

        private int VolumeTextureWidth;
        private int VolumeTextureHeight;
        private short VolumeTextureDepth;

        private int TransferFunctionWidth;

        const int TexturePixelSize = 4;	// The number of bytes used to represent a pixel in the texture.

        public HelloTexture(Volume volume, VolumeRenderer renderer)
        {
            this.volume = volume;
            this.renderer = renderer;

            VolumeTextureWidth = volume.XSize;
            VolumeTextureHeight = volume.YSize;
            VolumeTextureDepth = (short)volume.ZSize;

            TransferFunctionWidth = (int)(1 / TransferFunction.LookupTablePrecision);
        }

        /// <summary>
        /// Initialise pipeline and assets
        /// </summary>
        /// <param name="form">The form</param>
        public void Initialize(RenderForm form)
        {

            LoadPipeline(form);
            LoadAssets();
        }

        private void LoadPipeline(RenderForm form)
        {
            int width = form.ClientSize.Width;
            int height = form.ClientSize.Height;

            viewport.Width = width;
            viewport.Height = height;
            viewport.MaxDepth = 1.0f;

            scissorRect.Right = width;
            scissorRect.Bottom = height;

#if DEBUG
            // Enable the D3D12 debug layer.
            {
                DebugInterface.Get().EnableDebugLayer();
            }
#endif
            device = new Device(null, SharpDX.Direct3D.FeatureLevel.Level_11_0);
            using (var factory = new Factory4())
            {
                // Describe and create the command queue.
                var queueDesc = new CommandQueueDescription(CommandListType.Direct);
                commandQueue = device.CreateCommandQueue(queueDesc);


                // Describe and create the swap chain.
                var swapChainDesc = new SwapChainDescription()
                {
                    BufferCount = FrameCount,
                    ModeDescription = new ModeDescription(width, height, new Rational(60, 1), Format.R8G8B8A8_UNorm),
                    Usage = Usage.RenderTargetOutput,
                    SwapEffect = SwapEffect.FlipDiscard,
                    OutputHandle = form.Handle,
                    //Flags = SwapChainFlags.None,
                    SampleDescription = new SampleDescription(1, 0),
                    IsWindowed = true
                };

                var tempSwapChain = new SwapChain(factory, commandQueue, swapChainDesc);
                swapChain = tempSwapChain.QueryInterface<SwapChain3>();
                tempSwapChain.Dispose();
                frameIndex = swapChain.CurrentBackBufferIndex;
            }

            // Create descriptor heaps.
            // Describe and create a render target view (RTV) descriptor heap.
            var rtvHeapDesc = new DescriptorHeapDescription()
            {
                DescriptorCount = FrameCount,
                Flags = DescriptorHeapFlags.None,
                Type = DescriptorHeapType.RenderTargetView
            };

            renderTargetViewHeap = device.CreateDescriptorHeap(rtvHeapDesc);

            var srvHeapDesc = new DescriptorHeapDescription()
            {
                DescriptorCount = 2,
                Flags = DescriptorHeapFlags.ShaderVisible,
                Type = DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView
            };

            shaderRenderViewHeap = device.CreateDescriptorHeap(srvHeapDesc);

            // Describe and create a constant buffer view (CBV) descriptor heap.
            // Flags indicate that this descriptor heap can be bound to the pipeline 
            // and that descriptors contained in it can be referenced by a root table.
            var cbvHeapDesc = new DescriptorHeapDescription()
            {
                DescriptorCount = 1,
                Flags = DescriptorHeapFlags.ShaderVisible,
                Type = DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView
            };

            constantBufferViewHeap = device.CreateDescriptorHeap(cbvHeapDesc);

            rtvDescriptorSize = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.RenderTargetView);

            // Create frame resources.
            var rtvHandle = renderTargetViewHeap.CPUDescriptorHandleForHeapStart;
            for (int n = 0; n < FrameCount; n++)
            {
                renderTargets[n] = swapChain.GetBackBuffer<Resource>(n);
                device.CreateRenderTargetView(renderTargets[n], null, rtvHandle);
                rtvHandle += rtvDescriptorSize;
            }

            commandAllocator = device.CreateCommandAllocator(CommandListType.Direct);
        }

        private void LoadAssets()
        {
            // Create the root signature description.
            var rootSignatureDesc = new RootSignatureDescription(RootSignatureFlags.AllowInputAssemblerInputLayout, 
                // Root Parameters
                new[]
                {
                    new RootParameter(ShaderVisibility.Pixel,
                        new DescriptorRange()
                        {
                            RangeType = DescriptorRangeType.ShaderResourceView,
                            DescriptorCount = 2,
                            OffsetInDescriptorsFromTableStart = int.MinValue,
                            BaseShaderRegister = 0
                        }),
                    new RootParameter(ShaderVisibility.Pixel,
                        new DescriptorRange()
                        {
                            RangeType = DescriptorRangeType.ConstantBufferView,
                            DescriptorCount = 1,
                            BaseShaderRegister = 0,
                            OffsetInDescriptorsFromTableStart = int.MinValue,
                        })
                },
                // Samplers
                new[]
                {
                    new StaticSamplerDescription(ShaderVisibility.Pixel, 0, 0)
                    {
                        //Filter = Filter.MinimumMinMagMipPoint,
                        Filter = Filter.ComparisonMinLinearMagPointMipLinear,
                        AddressUVW = TextureAddressMode.Border,
                    }
                });

            rootSignature = device.CreateRootSignature(0, rootSignatureDesc.Serialize());

            // Create the pipeline state, which includes compiling and loading shaders.

            var vertexShaderPath = Utils.GetVertexShaderPath();
            var pixelShaderPath = Utils.GetPixelShaderForRenderingMode(VolViz.Configuration.RenderingMode.Dvr);

#if DEBUG
            var vertexShader = new ShaderBytecode(SharpDX.D3DCompiler.ShaderBytecode.CompileFromFile(vertexShaderPath, "VSMain", "vs_5_0", SharpDX.D3DCompiler.ShaderFlags.Debug));
#else
            var vertexShader = new ShaderBytecode(SharpDX.D3DCompiler.ShaderBytecode.CompileFromFile(vertexShaderPath, "VSMain", "vs_5_0"));
#endif

#if DEBUG
            var pixelShader = new ShaderBytecode(SharpDX.D3DCompiler.ShaderBytecode.CompileFromFile(pixelShaderPath, "PSMain", "ps_5_0", SharpDX.D3DCompiler.ShaderFlags.Debug));
#else
            var pixelShader = new ShaderBytecode(SharpDX.D3DCompiler.ShaderBytecode.CompileFromFile(pixelShaderPath, "PSMain", "ps_5_0"));
#endif

            // Define the vertex input layout.
            var inputElementDescs = new[]
            {
                    new InputElement("POSITION",0,Format.R32G32B32_Float,0,0),
                    new InputElement("TEXCOORD",0,Format.R32G32_Float,12,0)
            };

            // Describe and create the graphics pipeline state object (PSO).
            var psoDesc = new GraphicsPipelineStateDescription()
            {
                InputLayout = new InputLayoutDescription(inputElementDescs),
                RootSignature = rootSignature,
                VertexShader = vertexShader,
                PixelShader = pixelShader,
                RasterizerState = RasterizerStateDescription.Default(),
                BlendState = BlendStateDescription.Default(),
                DepthStencilFormat = SharpDX.DXGI.Format.D32_Float,
                DepthStencilState = new DepthStencilStateDescription() { IsDepthEnabled = false, IsStencilEnabled = false },
                SampleMask = int.MaxValue,
                PrimitiveTopologyType = PrimitiveTopologyType.Triangle,
                RenderTargetCount = 1,
                Flags = PipelineStateFlags.None,
                SampleDescription = new SharpDX.DXGI.SampleDescription(1, 0),
                StreamOutput = new StreamOutputDescription()
            };
            psoDesc.RenderTargetFormats[0] = SharpDX.DXGI.Format.R8G8B8A8_UNorm;

            pipelineState = device.CreateGraphicsPipelineState(psoDesc);

            // Create the command list.
            commandList = device.CreateCommandList(CommandListType.Direct, commandAllocator, pipelineState);

            // Define the geometry for a square consisting of two triangles.
            var squareVertexes = new[]
            {
                new Vertex() { Position = new Vector3(-1.0f, 1.0f , 0.0f),  TexCoord = new Vector2(0.0f, 0.0f) },
                new Vertex() { Position = new Vector3(1.0f, 1.0f, 0.0f), TexCoord = new Vector2(1.0f, 0.0f) },
                new Vertex() { Position = new Vector3(-1.0f, -1.0f, 0.0f),TexCoord = new Vector2(0.0f, 1.0f) },

                new Vertex() { Position = new Vector3(1.0f, 1.0f , 0.0f),  TexCoord = new Vector2(1.0f, 0.0f) },
                new Vertex() { Position = new Vector3(1.0f, -1.0f, 0.0f), TexCoord = new Vector2(1.0f, 1.0f) },
                new Vertex() { Position = new Vector3(-1.0f, -1.0f, 0.0f),TexCoord = new Vector2(0.0f, 1.0f) },
            };

            // Create the vertex buffer.
            int vertexBufferSize = Utilities.SizeOf(squareVertexes);

            // Note: using upload heaps to transfer static data like vert buffers is not 
            // recommended. Every time the GPU needs it, the upload heap will be marshalled 
            // over. Please read up on Default Heap usage. An upload heap is used here for 
            // code simplicity and because there are very few verts to actually transfer.
            vertexBuffer = device.CreateCommittedResource(new HeapProperties(HeapType.Upload), HeapFlags.None, ResourceDescription.Buffer(vertexBufferSize), ResourceStates.GenericRead);

            // Copy the triangle data to the vertex buffer.
            var pVertexDataBegin = vertexBuffer.Map(0);
            Utilities.Write(pVertexDataBegin, squareVertexes, 0, squareVertexes.Length);
            vertexBuffer.Unmap(0);

            // Initialize the vertex buffer view.
            vertexBufferView = new VertexBufferView();
            vertexBufferView.BufferLocation = vertexBuffer.GPUVirtualAddress;
            vertexBufferView.StrideInBytes = Utilities.SizeOf<Vertex>();
            vertexBufferView.SizeInBytes = vertexBufferSize;

            // Load volume data
            var volumeTextureDesc = ResourceDescription.Texture3D(Format.R8G8B8A8_UNorm, VolumeTextureWidth, VolumeTextureHeight, VolumeTextureDepth, 1);
            volumeTexture = device.CreateCommittedResource(new HeapProperties(
                HeapType.Default), 
                HeapFlags.None, 
                volumeTextureDesc, 
                ResourceStates.CopyDestination);
            
            long volumeUploadBufferSize = GetRequiredIntermediateSize(this.volumeTexture, 0, 1);

            // Create the GPU upload buffer.
            var volumeTextureUploadHeap = device.CreateCommittedResource(new HeapProperties(
                CpuPageProperty.WriteBack, 
                MemoryPool.L0), HeapFlags.None, 
                ResourceDescription.Texture3D(Format.R8G8B8A8_UNorm, VolumeTextureWidth, VolumeTextureHeight, VolumeTextureDepth, 1), 
                ResourceStates.GenericRead);

            // Copy data to the intermediate upload heap and then schedule a copy 
            // from the upload heap to the Texture3D.
            byte[] volumeTextureData = GenerateVolumeTextureData();

            var volumeHandle = GCHandle.Alloc(volumeTextureData, GCHandleType.Pinned);
            var volumePtr = Marshal.UnsafeAddrOfPinnedArrayElement(volumeTextureData, 0);

            volumeTextureUploadHeap.WriteToSubresource(0, null, volumePtr, TexturePixelSize * VolumeTextureWidth, TexturePixelSize * VolumeTextureWidth * VolumeTextureHeight);

            volumeHandle.Free();

            commandList.CopyTextureRegion(new TextureCopyLocation(volumeTexture, 0), 0, 0, 0, new TextureCopyLocation(volumeTextureUploadHeap, 0), null);
            commandList.ResourceBarrierTransition(this.volumeTexture, ResourceStates.CopyDestination, ResourceStates.PixelShaderResource);

            // Describe and create a SRV for the volume texture.
            var volumeSrvDesc = new ShaderResourceViewDescription
            {
                Shader4ComponentMapping = D3DXUtilities.DefaultComponentMapping(),
                Format = volumeTextureDesc.Format,
                Dimension = ShaderResourceViewDimension.Texture3D,
                Texture3D = { MipLevels = 1 },
            };

            device.CreateShaderResourceView(this.volumeTexture, volumeSrvDesc, shaderRenderViewHeap.CPUDescriptorHandleForHeapStart);
            // End load volume data

            // Load transfer function
            var transferFunctionTextureDesc = ResourceDescription.Texture1D(Format.R8G8B8A8_UNorm, TransferFunctionWidth, 1);
            transferFunctionTexture = device.CreateCommittedResource(new HeapProperties(
                HeapType.Default),
                HeapFlags.None,
                transferFunctionTextureDesc,
                ResourceStates.CopyDestination);

            long transferFunctionUploadBufferSize = GetRequiredIntermediateSize(this.transferFunctionTexture, 0, 1);

            // Create the GPU upload buffer.
            var transferFunctionTextureUploadHeap = device.CreateCommittedResource(new HeapProperties(
                CpuPageProperty.WriteBack,
                MemoryPool.L0), HeapFlags.None,
                ResourceDescription.Texture1D(Format.R8G8B8A8_UNorm, TransferFunctionWidth, 1),
                ResourceStates.GenericRead);

            // Copy data to the intermediate upload heap and then schedule a copy 
            // from the upload heap to the Texture3D.
            byte[] transferFunctionTextureData = GenerateTransferFunctionTextureData();

            var transferFunctionHandle = GCHandle.Alloc(transferFunctionTextureData, GCHandleType.Pinned);
            var transferFunctionPtr = Marshal.UnsafeAddrOfPinnedArrayElement(transferFunctionTextureData, 0);

            transferFunctionTextureUploadHeap.WriteToSubresource(0, null, transferFunctionPtr, TransferFunctionWidth, TransferFunctionWidth);

            transferFunctionHandle.Free();

            commandList.CopyTextureRegion(new TextureCopyLocation(transferFunctionTexture, 0), 0, 0, 0, new TextureCopyLocation(transferFunctionTextureUploadHeap, 0), null);
            commandList.ResourceBarrierTransition(this.transferFunctionTexture, ResourceStates.CopyDestination, ResourceStates.PixelShaderResource);

            // Describe and create a SRV for the transfer function texture.
            var transferFunctionSrvDesc = new ShaderResourceViewDescription
            {
                Shader4ComponentMapping = D3DXUtilities.DefaultComponentMapping(),
                Format = transferFunctionTextureDesc.Format,
                Dimension = ShaderResourceViewDimension.Texture1D,
                Texture1D = { MipLevels = 1 },
            };

            var handleIncrement = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView);
            
            CpuDescriptorHandle locationDesctiptor = shaderRenderViewHeap.CPUDescriptorHandleForHeapStart + handleIncrement;
            device.CreateShaderResourceView(this.transferFunctionTexture, transferFunctionSrvDesc, locationDesctiptor);
            // End load transfer function data

            // Command lists are created in the recording state, but there is nothing
            // to record yet. The main loop expects it to be closed, so close it now.
            commandList.Close();

            constantBuffer = device.CreateCommittedResource(new HeapProperties(HeapType.Upload), HeapFlags.None, ResourceDescription.Buffer(1024 * 64), ResourceStates.GenericRead);

            //// Describe and create a constant buffer view.
            var cbvDesc = new ConstantBufferViewDescription
            {
                BufferLocation = constantBuffer.GPUVirtualAddress,
                SizeInBytes = (Utilities.SizeOf<ConstantBuffer>() + 255) & ~255
            };

            device.CreateConstantBufferView(cbvDesc, constantBufferViewHeap.CPUDescriptorHandleForHeapStart);

            // Initialize and map the constant buffers. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            constantBufferPointer = constantBuffer.Map(0);
            Utilities.Write(constantBufferPointer, ref constantBufferData);

            commandQueue.ExecuteCommandList(commandList);

            // Create synchronization objects.
            fence = device.CreateFence(0, FenceFlags.None);
            fenceValue = 1;

            // Create an event handle to use for frame synchronization.
            fenceEvent = new AutoResetEvent(false);

            WaitForPreviousFrame();

            //release temp texture
            volumeTextureUploadHeap.Dispose();
            transferFunctionTextureUploadHeap.Dispose();
        }

        byte[] GenerateVolumeTextureData()
        {
            int rowPitch = VolumeTextureWidth * TexturePixelSize;
            int layerPitch = rowPitch * VolumeTextureHeight;

            int layerSize = VolumeTextureWidth * VolumeTextureHeight;
            int textureSize = layerPitch * VolumeTextureDepth;


            byte[] data = new byte[textureSize];

            for (int n = 0; n < textureSize; n += TexturePixelSize)
            {
                int x = (n / 4) % VolumeTextureWidth;
                int y = ((n / 4) % layerSize) / VolumeTextureWidth;
                int z = (n / 4) / layerSize;

                var voxelValue = volume.Contents[x, y, z];

                data[n] = (byte)(voxelValue * 255);
                data[n + 1] = (byte)(voxelValue * 255);
                data[n + 2] = (byte)(voxelValue * 255);
                data[n + 3] = 0xff;
            }

            return data;
        }

        byte[] GenerateTransferFunctionTextureData()
        {
            // TODO: Should create new, correct sampler such that pixel shader doesn't interpolate towards area outside the texture

            int textureSize = TransferFunctionWidth * TexturePixelSize;

            byte[] data = new byte[textureSize];

            for (int n = 0; n < textureSize; n += TexturePixelSize)
            {
                // TODO: There's a +/- 1 error here, since the lookup table is 1 space longer than one would
                // expect: it spans from 0 to 1 in increments of LookupTablePrecision. If this is not fixed,
                // the highest value of the lookup table will not be included in this texture, which is a minor problem.

                var intensity = n / (float)textureSize;

                var color = renderer.TransferFunction.GetColorOfIntensity(intensity, useLookupTable: true);
                var opacity = renderer.TransferFunction.GetOpacityOfIntensity(intensity, useLookupTable: true);

                data[n] = (byte)(color.X * 255); // R
                data[n + 1] = (byte)(color.Y * 255); // G
                data[n + 2] = (byte)(color.Z * 255); // B
                data[n + 3] = (byte)(opacity * 255); // A
            }

            return data;
        }

        private long GetRequiredIntermediateSize(Resource destinationResource, int firstSubresource, int NumSubresources)
        {
            var desc = destinationResource.Description;
            long requiredSize;
            device.GetCopyableFootprints(ref desc, firstSubresource, NumSubresources, 0, null, null, null, out requiredSize);
            return requiredSize;
        }

        private void PopulateCommandList()
        {
            // Command list allocators can only be reset when the associated 
            // command lists have finished execution on the GPU; apps should use 
            // fences to determine GPU execution progress.
            commandAllocator.Reset();

            // However, when ExecuteCommandList() is called on a particular command 
            // list, that command list can then be reset at any time and must be before 
            // re-recording.
            commandList.Reset(commandAllocator, pipelineState);


            // Set necessary state.
            commandList.SetGraphicsRootSignature(rootSignature);

            commandList.SetDescriptorHeaps(1, new DescriptorHeap[] { shaderRenderViewHeap });
            commandList.SetGraphicsRootDescriptorTable(0, shaderRenderViewHeap.GPUDescriptorHandleForHeapStart);

            commandList.SetDescriptorHeaps(1, new DescriptorHeap[] { constantBufferViewHeap });
            commandList.SetGraphicsRootDescriptorTable(1, constantBufferViewHeap.GPUDescriptorHandleForHeapStart);

            UpdateTransferFunctionTexture();

            commandList.SetViewport(viewport);
            commandList.SetScissorRectangles(scissorRect);

            // Indicate that the back buffer will be used as a render target.
            commandList.ResourceBarrierTransition(renderTargets[frameIndex], ResourceStates.Present, ResourceStates.RenderTarget);

            var rtvHandle = renderTargetViewHeap.CPUDescriptorHandleForHeapStart;
            rtvHandle += frameIndex * rtvDescriptorSize;
            commandList.SetRenderTargets(rtvHandle, null);

            // Record commands.
            commandList.ClearRenderTargetView(rtvHandle, new Color4(0, 0.2F, 0.4f, 1), 0, null);

            commandList.PrimitiveTopology = SharpDX.Direct3D.PrimitiveTopology.TriangleList;
            commandList.SetVertexBuffer(0, vertexBufferView);
            commandList.DrawInstanced(6, 1, 0, 0);

            // Indicate that the back buffer will now be used to present.
            commandList.ResourceBarrierTransition(renderTargets[frameIndex], ResourceStates.RenderTarget, ResourceStates.Present);

            commandList.Close();
        }


        /// <summary> 
        /// Wait the previous command list to finish executing. 
        /// </summary> 
        private void WaitForPreviousFrame()
        {
            // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE. 
            // This is code implemented as such for simplicity. 

            int localFence = fenceValue;
            commandQueue.Signal(this.fence, localFence);
            fenceValue++;

            // Wait until the previous frame is finished.
            if (this.fence.CompletedValue < localFence)
            {
                this.fence.SetEventOnCompletion(localFence, fenceEvent.SafeWaitHandle.DangerousGetHandle());
                fenceEvent.WaitOne();
            }

            frameIndex = swapChain.CurrentBackBufferIndex;
        }

        public void Update()
        {
            const float translationSpeed = 0.005f;
            const float offsetBounds = 1.25f;

            constantBufferData.Offset.X += translationSpeed;
            if (constantBufferData.Offset.X > offsetBounds)
            {
                constantBufferData.Offset.X = -offsetBounds;
            }

            constantBufferData.ProjectionDirection.X = renderer.Viewport.ProjectionDirection.X;
            constantBufferData.ProjectionDirection.Y = renderer.Viewport.ProjectionDirection.Y;
            constantBufferData.ProjectionDirection.Z = renderer.Viewport.ProjectionDirection.Z;

            constantBufferData.BottomLeft.X = renderer.Viewport.BottomLeft.X;
            constantBufferData.BottomLeft.Y = renderer.Viewport.BottomLeft.Y;
            constantBufferData.BottomLeft.Z = renderer.Viewport.BottomLeft.Z;

            constantBufferData.RightSpan.X = renderer.Viewport.RightSpan.X;
            constantBufferData.RightSpan.Y = renderer.Viewport.RightSpan.Y;
            constantBufferData.RightSpan.Z = renderer.Viewport.RightSpan.Z;

            constantBufferData.UpSpan.X = renderer.Viewport.UpSpan.X;
            constantBufferData.UpSpan.Y = renderer.Viewport.UpSpan.Y;
            constantBufferData.UpSpan.Z = renderer.Viewport.UpSpan.Z;

            // Since sampling of texture data is always from [0,1] rather than dimension size, must compensate
            // for the fact that dataset dimensions have different size when sampling.
            constantBufferData.DimensionSizeFactors.X = volume.XSize / (float)volume.SizeOfLargestDimension;
            constantBufferData.DimensionSizeFactors.Y = volume.YSize / (float)volume.SizeOfLargestDimension;
            constantBufferData.DimensionSizeFactors.Z = volume.ZSize / (float)volume.SizeOfLargestDimension;
            constantBufferData.DimensionSizeFactors.W = volume.SizeOfLargestDimension;
            
            constantBufferData.StepSize = 0.01f;

            Vector4 firstRayPosition =
                constantBufferData.BottomLeft +
                constantBufferData.RightSpan +
                constantBufferData.UpSpan +
                constantBufferData.ProjectionDirection * constantBufferData.Offset.X;

            Utilities.Write(constantBufferPointer, ref constantBufferData);
        }
        
        public void Render()
        {
            // Record all the commands we need to render the scene into the command list.
            PopulateCommandList();

            // Execute the command list.
            commandQueue.ExecuteCommandList(commandList);

            // Present the frame.
            swapChain.Present(1, 0);

            WaitForPreviousFrame();

            // Upload heap was set during command list population and may be disposed now that the frame has been drawn
            transferFunctionTextureUploadHeap.Dispose();
        }

        public void Dispose()
        {
            // Wait for the GPU to be done with all resources.
            WaitForPreviousFrame();

            //release all resources
            foreach (var target in renderTargets)
            {
                target.Dispose();
            }
            commandAllocator.Dispose();
            commandQueue.Dispose();
            rootSignature.Dispose();
            renderTargetViewHeap.Dispose();
            shaderRenderViewHeap.Dispose();
            constantBufferViewHeap.Dispose();
            pipelineState.Dispose();
            commandList.Dispose();
            vertexBuffer.Dispose();
            constantBuffer.Dispose();
            volumeTexture.Dispose();
            fence.Dispose();
            swapChain.Dispose();
            device.Dispose();
        }

        public void UpdateTransferFunctionTexture()
        {
            // TODO: This code is identical with TF loading in the LoadAssets method.
            // Could be extracted to common helper class, perhaps also including
            // volume texture loading.

            // TODO: I suspect the might be another, minor, memory leak here.

            // Load transfer function
            var transferFunctionTextureDesc = ResourceDescription.Texture1D(Format.R8G8B8A8_UNorm, TransferFunctionWidth, 1);
            transferFunctionTexture = device.CreateCommittedResource(new HeapProperties(
                HeapType.Default),
                HeapFlags.None,
                transferFunctionTextureDesc,
                ResourceStates.CopyDestination);

            long transferFunctionUploadBufferSize = GetRequiredIntermediateSize(this.transferFunctionTexture, 0, 1);

            // Create the GPU upload buffer.
            transferFunctionTextureUploadHeap = device.CreateCommittedResource(new HeapProperties(
                CpuPageProperty.WriteBack,
                MemoryPool.L0), HeapFlags.None,
                ResourceDescription.Texture1D(Format.R8G8B8A8_UNorm, TransferFunctionWidth, 1),
                ResourceStates.GenericRead);

            // Copy data to the intermediate upload heap and then schedule a copy 
            // from the upload heap to the Texture3D.
            byte[] transferFunctionTextureData = GenerateTransferFunctionTextureData();

            var transferFunctionHandle = GCHandle.Alloc(transferFunctionTextureData, GCHandleType.Pinned);
            var transferFunctionPtr = Marshal.UnsafeAddrOfPinnedArrayElement(transferFunctionTextureData, 0);

            transferFunctionTextureUploadHeap.WriteToSubresource(0, null, transferFunctionPtr, TransferFunctionWidth, TransferFunctionWidth);

            transferFunctionHandle.Free();

            commandList.CopyTextureRegion(new TextureCopyLocation(transferFunctionTexture, 0), 0, 0, 0, new TextureCopyLocation(transferFunctionTextureUploadHeap, 0), null);
            commandList.ResourceBarrierTransition(this.transferFunctionTexture, ResourceStates.CopyDestination, ResourceStates.PixelShaderResource);

            // Describe and create a SRV for the transfer function texture.
            var transferFunctionSrvDesc = new ShaderResourceViewDescription
            {
                Shader4ComponentMapping = D3DXUtilities.DefaultComponentMapping(),
                Format = transferFunctionTextureDesc.Format,
                Dimension = ShaderResourceViewDimension.Texture1D,
                Texture1D = { MipLevels = 1 },
            };

            var handleIncrement = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView);

            CpuDescriptorHandle locationDesctiptor = shaderRenderViewHeap.CPUDescriptorHandleForHeapStart + handleIncrement;
            device.CreateShaderResourceView(this.transferFunctionTexture, transferFunctionSrvDesc, locationDesctiptor);
            // End load transfer function data
        }

        struct Vertex
        {
            public Vector3 Position;

            public Vector2 TexCoord;
        };

        struct ConstantBuffer
        {
            public Vector4 Offset;

            public Vector4 ProjectionDirection;
            public Vector4 BottomLeft;
            public Vector4 RightSpan;
            public Vector4 UpSpan;

            // Factors by with to reduce the size of each dimension when sampling (1 for largest dimension)
            public Vector4 DimensionSizeFactors; 
            public float StepSize;
        };

        const int FrameCount = 2;

        private ViewportF viewport;
        private Rectangle scissorRect;
        // Pipeline objects.
        private SwapChain3 swapChain;
        private Device device;
        private Resource[] renderTargets = new Resource[FrameCount];
        private CommandAllocator commandAllocator;
        private CommandQueue commandQueue;
        private RootSignature rootSignature;
        private DescriptorHeap renderTargetViewHeap;
        private DescriptorHeap shaderRenderViewHeap;
        private DescriptorHeap constantBufferViewHeap;
        private PipelineState pipelineState;
        private GraphicsCommandList commandList;
        private int rtvDescriptorSize;

        // App resources.
        Resource vertexBuffer;
        VertexBufferView vertexBufferView;
        Resource volumeTexture;
        Resource transferFunctionTexture;
        Resource constantBuffer;
        ConstantBuffer constantBufferData;
        IntPtr constantBufferPointer;

        // Keep this in a common variable, so it can be safely disposed whenever necessary
        Resource transferFunctionTextureUploadHeap;

        // Synchronization objects.
        private int frameIndex;
        private AutoResetEvent fenceEvent;

        private Fence fence;
        private int fenceValue;

    }
}
