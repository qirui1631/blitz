#ifndef SRC_BACKENDS_CPU_BACKEND_DISPATCH_INL_H_
#define SRC_BACKENDS_CPU_BACKEND_DISPATCH_INL_H_

template<typename DType>
void Backend<CPUTensor, DType>::ConvolutionForwardGEMMDispatch(
	DType* unpack,
	DType* output,
	DType* filter,
	size_t K, size_t PQ, size_t CRS,
	BLITZ_DATA_LAYOUT unpack_data_layout,
	BLITZ_DATA_LAYOUT output_data_layout,
	BLITZ_DATA_LAYOUT filter_data_layout) {
	if (unpack_data_layout == BLITZ_PACK_PQCRS) {
		if (output_data_layout == BLITZ_BUFFER_NCHW) {
			BlitzCPUGemm(filter,
				unpack,
				output,
				false, true,
				static_cast<DType>(1), static_cast<DType>(0),
				K, PQ, CRS);
		} else if (output_data_layout == BLITZ_BUFFER_NHWC) {
			BlitzCPUGemm(unpack,
				filter,
				output,
				false, true,
				static_cast<DType>(1), static_cast<DType>(0),
				PQ, K, CRS);
		} else {
			LOG(FATAL) << "Unsupported layout combination: " << unpack_data_layout <<
				" and " << output_data_layout;
		}
	} else if (unpack_data_layout == BLITZ_PACK_CRSPQ) {
		if (output_data_layout == BLITZ_BUFFER_NCHW) {
			BlitzCPUGemm(filter,
				unpack,
				output,
				false, false,
				static_cast<DType>(1), static_cast<DType>(0),
				K, PQ, CRS);
		} else if (output_data_layout == BLITZ_BUFFER_NHWC) {
			BlitzCPUGemm(unpack,
				filter,
				output,
				true, true,
				static_cast<DType>(1), static_cast<DType>(0),
				PQ, K, CRS);
		} else {
			LOG(FATAL) << "Unsupported layout combination: " << unpack_data_layout <<
				" and " << output_data_layout;
		}
	} else {
		LOG(FATAL) << "Unsupported layout type: " << unpack_data_layout;
	}
}

#endif  // SRC_BACKENDS_CPU_BACKEND_DISPATCH_INL_H_
