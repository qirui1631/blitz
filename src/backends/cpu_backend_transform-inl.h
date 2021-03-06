#ifndef SRC_BACKENDS_CPU_BACKEND_TRANSFORM_INL_H_
#define SRC_BACKENDS_CPU_BACKEND_TRANSFORM_INL_H_

static void Unpack2DFunc(
  const CPUTensor<DType>* input,
  CPUTensor<DType>* unpack,
  size_t R, size_t S,
  size_t pad_h, size_t pad_w,
  size_t str_h, size_t str_w) {
  size_t N, C, H, W;
  size_t P, Q;
  Blitz2DBuffer(input->shape(), &N, &C, &H, &W);
  P = (H + 2 * pad_h - R) / str_h + 1;
  Q = (W + 2 * pad_w - S) / str_w + 1;
  CHECK_EQ(unpack->size(), N * C * R * S * P * Q);
  for (size_t i = 0; i < N; ++i) {
    utils::Unpack2DDispatch<CPUTensor, DType>(
      input->data(), unpack->data(),
      C, H, W,
      R, S,
      P, Q,
      pad_h, pad_w,
      str_h, str_w,
      input->data_layout());
  }
}

static void Pack2DFunc(
  const CPUTensor<DType>* unpack,
  CPUTensor<DType>* input,
  size_t R, size_t S,
  size_t pad_h, size_t pad_w,
  size_t str_h, size_t str_w) {
  size_t N, C, H, W;
  size_t P, Q;
  Blitz2DBuffer(input->shape(), &N, &C, &H, &W);
  P = (H + 2 * pad_h - R) / str_h + 1;
  Q = (W + 2 * pad_w - S) / str_w + 1;
  CHECK_EQ(unpack->size(), N * C * R * S * P * Q);
  for (size_t i = 0; i < N; ++i) {
    utils::Pack2DDispatch<CPUTensor, DType>(
      unpack->data(), input->data(),
      C, H, W,
      R, S,
      P, Q,
      pad_h, pad_w,
      str_h, str_w,
      input->data_layout());
  }
}

static void TransformCopyFunc(
  const CPUTensor<DType>* source, CPUTensor<DType>* dest) {
  if (source->size() != dest->size()) {
    LOG(FATAL) << "Tensor size do not match!";
  }
  if (source->data_layout() == BLITZ_BUFFER_NCHW) {
    if (dest->data_layout() == BLITZ_BUFFER_NHWC) {
      utils::TransformBufferImpl<CPUTensor, DType, BLITZ_BUFFER_NCHW, BLITZ_BUFFER_NHWC>(
        source->data(), dest->data(),
        source->shape()[0], source->shape()[1], source->shape()[2], source->shape()[3]);
      return;
    }
  } else if (source->data_layout() == BLITZ_BUFFER_NHWC) {
    if (dest->data_layout() == BLITZ_BUFFER_NCHW) {
      utils::TransformBufferImpl<CPUTensor, DType, BLITZ_BUFFER_NHWC, BLITZ_BUFFER_NCHW>(
        source->data(), dest->data(),
        dest->shape()[0], dest->shape()[1], dest->shape()[2], dest->shape()[3]);
      return;
    }
  } else if (source->data_layout() == BLITZ_FILTER_KCRS) {
    if (dest->data_layout() == BLITZ_FILTER_RSCK) {
      utils::TransformFilterImpl<CPUTensor, DType, BLITZ_FILTER_KCRS, BLITZ_FILTER_RSCK>(
        source->data(), dest->data(),
        source->shape()[0], source->shape()[1], source->shape()[2], source->shape()[3]);
      return;
    }
  } else if (source->data_layout() == BLITZ_FILTER_RSCK) {
    if (dest->data_layout() == BLITZ_FILTER_KCRS) {
      utils::TransformFilterImpl<CPUTensor, DType, BLITZ_FILTER_RSCK, BLITZ_FILTER_KCRS>(
        source->data(), dest->data(),
        dest->shape()[0], dest->shape()[1], dest->shape()[2], dest->shape()[3]);
      return;
    }
  }
  utils::CPUCopy(source->data(), dest->data(), source->size());
}

static void HostCopyToTensorFunc(
  const DType *host,
  CPUTensor<DType> *tensor) {
  utils::CPUCopy(host, tensor->data(), tensor->size());
}

#endif  // SRC_BACKENDS_CPU_BACKEND_TRANSFORM_INL_H_
