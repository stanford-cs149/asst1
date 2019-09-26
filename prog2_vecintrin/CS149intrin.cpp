#include "CS149intrin.h"
#include "logger.h"

//******************
//* Implementation *
//******************

__cs149_mask _cs149_init_ones(int first) {
  __cs149_mask mask;
  for (int i=0; i<VECTOR_WIDTH; i++) {
    mask.value[i] = (i<first) ? true : false;
  }
  return mask;
}

__cs149_mask _cs149_mask_not(__cs149_mask &maska) {
  __cs149_mask resultMask;
  for (int i=0; i<VECTOR_WIDTH; i++) {
    resultMask.value[i] = !maska.value[i];
  }
  CS149Logger.addLog("masknot", _cs149_init_ones(), VECTOR_WIDTH);
  return resultMask;
}

__cs149_mask _cs149_mask_or(__cs149_mask &maska, __cs149_mask &maskb) {
  __cs149_mask resultMask;
  for (int i=0; i<VECTOR_WIDTH; i++) {
    resultMask.value[i] = maska.value[i] | maskb.value[i];
  }
  CS149Logger.addLog("maskor", _cs149_init_ones(), VECTOR_WIDTH);
  return resultMask;
}

__cs149_mask _cs149_mask_and(__cs149_mask &maska, __cs149_mask &maskb) {
  __cs149_mask resultMask;
  for (int i=0; i<VECTOR_WIDTH; i++) {
    resultMask.value[i] = maska.value[i] && maskb.value[i];
  }
  CS149Logger.addLog("maskand", _cs149_init_ones(), VECTOR_WIDTH);
  return resultMask;
}

int _cs149_cntbits(__cs149_mask &maska) {
  int count = 0;
  for (int i=0; i<VECTOR_WIDTH; i++) {
    if (maska.value[i]) count++;
  }
  CS149Logger.addLog("cntbits", _cs149_init_ones(), VECTOR_WIDTH);
  return count;
}

template <typename T>
void _cs149_vset(__cs149_vec<T> &vecResult, T value, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? value : vecResult.value[i];
  }
  CS149Logger.addLog("vset", mask, VECTOR_WIDTH);
}

template void _cs149_vset<float>(__cs149_vec_float &vecResult, float value, __cs149_mask &mask);
template void _cs149_vset<int>(__cs149_vec_int &vecResult, int value, __cs149_mask &mask);

void _cs149_vset_float(__cs149_vec_float &vecResult, float value, __cs149_mask &mask) { _cs149_vset<float>(vecResult, value, mask); }
void _cs149_vset_int(__cs149_vec_int &vecResult, int value, __cs149_mask &mask) { _cs149_vset<int>(vecResult, value, mask); }

__cs149_vec_float _cs149_vset_float(float value) {
  __cs149_vec_float vecResult;
  __cs149_mask mask = _cs149_init_ones();
  _cs149_vset_float(vecResult, value, mask);
  return vecResult;
}
__cs149_vec_int _cs149_vset_int(int value) {
  __cs149_vec_int vecResult;
  __cs149_mask mask = _cs149_init_ones();
  _cs149_vset_int(vecResult, value, mask);
  return vecResult;
}

template <typename T>
void _cs149_vmove(__cs149_vec<T> &dest, __cs149_vec<T> &src, __cs149_mask &mask) {
    for (int i = 0; i < VECTOR_WIDTH; i++) {
        dest.value[i] = mask.value[i] ? src.value[i] : dest.value[i];
    }
    CS149Logger.addLog("vmove", mask, VECTOR_WIDTH);
}

template void _cs149_vmove<float>(__cs149_vec_float &dest, __cs149_vec_float &src, __cs149_mask &mask);
template void _cs149_vmove<int>(__cs149_vec_int &dest, __cs149_vec_int &src, __cs149_mask &mask);

void _cs149_vmove_float(__cs149_vec_float &dest, __cs149_vec_float &src, __cs149_mask &mask) { _cs149_vmove<float>(dest, src, mask); }
void _cs149_vmove_int(__cs149_vec_int &dest, __cs149_vec_int &src, __cs149_mask &mask) { _cs149_vmove<int>(dest, src, mask); }

template <typename T>
void _cs149_vload(__cs149_vec<T> &dest, T* src, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    dest.value[i] = mask.value[i] ? src[i] : dest.value[i];
  }
  CS149Logger.addLog("vload", mask, VECTOR_WIDTH);
}

template void _cs149_vload<float>(__cs149_vec_float &dest, float* src, __cs149_mask &mask);
template void _cs149_vload<int>(__cs149_vec_int &dest, int* src, __cs149_mask &mask);

void _cs149_vload_float(__cs149_vec_float &dest, float* src, __cs149_mask &mask) { _cs149_vload<float>(dest, src, mask); }
void _cs149_vload_int(__cs149_vec_int &dest, int* src, __cs149_mask &mask) { _cs149_vload<int>(dest, src, mask); }

template <typename T>
void _cs149_vstore(T* dest, __cs149_vec<T> &src, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    dest[i] = mask.value[i] ? src.value[i] : dest[i];
  }
  CS149Logger.addLog("vstore", mask, VECTOR_WIDTH);
}

template void _cs149_vstore<float>(float* dest, __cs149_vec_float &src, __cs149_mask &mask);
template void _cs149_vstore<int>(int* dest, __cs149_vec_int &src, __cs149_mask &mask);

void _cs149_vstore_float(float* dest, __cs149_vec_float &src, __cs149_mask &mask) { _cs149_vstore<float>(dest, src, mask); }
void _cs149_vstore_int(int* dest, __cs149_vec_int &src, __cs149_mask &mask) { _cs149_vstore<int>(dest, src, mask); }

template <typename T>
void _cs149_vadd(__cs149_vec<T> &vecResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? (veca.value[i] + vecb.value[i]) : vecResult.value[i];
  }
  CS149Logger.addLog("vadd", mask, VECTOR_WIDTH);
}

template void _cs149_vadd<float>(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vadd<int>(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vadd_float(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vadd<float>(vecResult, veca, vecb, mask); }
void _cs149_vadd_int(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vadd<int>(vecResult, veca, vecb, mask); }

template <typename T>
void _cs149_vsub(__cs149_vec<T> &vecResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? (veca.value[i] - vecb.value[i]) : vecResult.value[i];
  }
  CS149Logger.addLog("vsub", mask, VECTOR_WIDTH);
}

template void _cs149_vsub<float>(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vsub<int>(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vsub_float(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vsub<float>(vecResult, veca, vecb, mask); }
void _cs149_vsub_int(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vsub<int>(vecResult, veca, vecb, mask); }

template <typename T>
void _cs149_vmult(__cs149_vec<T> &vecResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? (veca.value[i] * vecb.value[i]) : vecResult.value[i];
  }
  CS149Logger.addLog("vmult", mask, VECTOR_WIDTH);
}

template void _cs149_vmult<float>(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vmult<int>(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vmult_float(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vmult<float>(vecResult, veca, vecb, mask); }
void _cs149_vmult_int(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vmult<int>(vecResult, veca, vecb, mask); }

template <typename T>
void _cs149_vdiv(__cs149_vec<T> &vecResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? (veca.value[i] / vecb.value[i]) : vecResult.value[i];
  }
  CS149Logger.addLog("vdiv", mask, VECTOR_WIDTH);
}

template void _cs149_vdiv<float>(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vdiv<int>(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vdiv_float(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vdiv<float>(vecResult, veca, vecb, mask); }
void _cs149_vdiv_int(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vdiv<int>(vecResult, veca, vecb, mask); }

template <typename T>
void _cs149_vabs(__cs149_vec<T> &vecResult, __cs149_vec<T> &veca, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    vecResult.value[i] = mask.value[i] ? (abs(veca.value[i])) : vecResult.value[i];
  }
  CS149Logger.addLog("vabs", mask, VECTOR_WIDTH);
}

template void _cs149_vabs<float>(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_mask &mask);
template void _cs149_vabs<int>(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_mask &mask);

void _cs149_vabs_float(__cs149_vec_float &vecResult, __cs149_vec_float &veca, __cs149_mask &mask) { _cs149_vabs<float>(vecResult, veca, mask); }
void _cs149_vabs_int(__cs149_vec_int &vecResult, __cs149_vec_int &veca, __cs149_mask &mask) { _cs149_vabs<int>(vecResult, veca, mask); }

template <typename T>
void _cs149_vgt(__cs149_mask &maskResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    maskResult.value[i] = mask.value[i] ? (veca.value[i] > vecb.value[i]) : maskResult.value[i];
  }
  CS149Logger.addLog("vgt", mask, VECTOR_WIDTH);
}

template void _cs149_vgt<float>(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vgt<int>(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vgt_float(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vgt<float>(maskResult, veca, vecb, mask); }
void _cs149_vgt_int(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vgt<int>(maskResult, veca, vecb, mask); }

template <typename T>
void _cs149_vlt(__cs149_mask &maskResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    maskResult.value[i] = mask.value[i] ? (veca.value[i] < vecb.value[i]) : maskResult.value[i];
  }
  CS149Logger.addLog("vlt", mask, VECTOR_WIDTH);
}

template void _cs149_vlt<float>(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_vlt<int>(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_vlt_float(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_vlt<float>(maskResult, veca, vecb, mask); }
void _cs149_vlt_int(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_vlt<int>(maskResult, veca, vecb, mask); }

template <typename T>
void _cs149_veq(__cs149_mask &maskResult, __cs149_vec<T> &veca, __cs149_vec<T> &vecb, __cs149_mask &mask) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    maskResult.value[i] = mask.value[i] ? (veca.value[i] == vecb.value[i]) : maskResult.value[i];
  }
  CS149Logger.addLog("veq", mask, VECTOR_WIDTH);
}

template void _cs149_veq<float>(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask);
template void _cs149_veq<int>(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask);

void _cs149_veq_float(__cs149_mask &maskResult, __cs149_vec_float &veca, __cs149_vec_float &vecb, __cs149_mask &mask) { _cs149_veq<float>(maskResult, veca, vecb, mask); }
void _cs149_veq_int(__cs149_mask &maskResult, __cs149_vec_int &veca, __cs149_vec_int &vecb, __cs149_mask &mask) { _cs149_veq<int>(maskResult, veca, vecb, mask); }

template <typename T>
void _cs149_hadd(__cs149_vec<T> &vecResult, __cs149_vec<T> &vec) {
  for (int i=0; i<VECTOR_WIDTH/2; i++) {
    T result = vec.value[2*i] + vec.value[2*i+1];
    vecResult.value[2 * i] = result;
    vecResult.value[2 * i + 1] = result;
  }
}

template void _cs149_hadd<float>(__cs149_vec_float &vecResult, __cs149_vec_float &vec);

void _cs149_hadd_float(__cs149_vec_float &vecResult, __cs149_vec_float &vec) { _cs149_hadd<float>(vecResult, vec); }

template <typename T>
void _cs149_interleave(__cs149_vec<T> &vecResult, __cs149_vec<T> &vec) {
  for (int i=0; i<VECTOR_WIDTH; i++) {
    int index = i < VECTOR_WIDTH/2 ? (2 * i) : (2 * (i - VECTOR_WIDTH/2) + 1);
    vecResult.value[i] = vec.value[index];
  }
}

template void _cs149_interleave<float>(__cs149_vec_float &vecResult, __cs149_vec_float &vec);

void _cs149_interleave_float(__cs149_vec_float &vecResult, __cs149_vec_float &vec) { _cs149_interleave<float>(vecResult, vec); }

void addUserLog(const char * logStr) {
  CS149Logger.addLog(logStr, _cs149_init_ones(), 0);
}

