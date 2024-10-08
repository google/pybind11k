#include <pybind11/pybind11.h>

#include "number_bucket.h"

#include <cstddef>
#include <memory>

namespace hc { // holder comparison

using nb_up = pybind11_ubench::number_bucket<1>;
using nb_sp = pybind11_ubench::number_bucket<2>;
using nb_pu = pybind11_ubench::number_bucket<3>;
using nb_sh = pybind11_ubench::number_bucket<4>;

namespace py = pybind11;

template <typename WrappedType, typename HolderType>
void wrap_number_bucket(py::module m, const char *class_name) {
    py::class_<WrappedType, HolderType>(m, class_name)
        .def(py::init<std::size_t>(), py::arg("data_size") = 0)
        .def("sum", &WrappedType::sum)
        .def("add", &WrappedType::add, py::arg("other"));
}

#ifdef PYBIND11_HAS_INTERNALS_WITH_SMART_HOLDER_SUPPORT

template <typename T>
class padded_unique_ptr {
    std::unique_ptr<T> ptr;
    char padding[sizeof(py::smart_holder) - sizeof(std::unique_ptr<T>)];

public:
    padded_unique_ptr(T *p) : ptr(p) {}
    T *get() { return ptr.get(); }
};

static_assert(sizeof(padded_unique_ptr<nb_pu>) == sizeof(py::smart_holder),
              "Unexpected sizeof mismatch.");

#endif

} // namespace hc

#ifdef PYBIND11_HAS_INTERNALS_WITH_SMART_HOLDER_SUPPORT
PYBIND11_DECLARE_HOLDER_TYPE(T, hc::padded_unique_ptr<T>);
#endif

PYBIND11_MODULE(pybind11_ubench_holder_comparison, m) {
    using namespace hc;
    wrap_number_bucket<nb_up, std::unique_ptr<nb_up>>(m, "number_bucket_up");
    wrap_number_bucket<nb_sp, std::shared_ptr<nb_sp>>(m, "number_bucket_sp");
#ifdef PYBIND11_HAS_INTERNALS_WITH_SMART_HOLDER_SUPPORT
    m.def("sizeof_smart_holder", []() { return sizeof(py::smart_holder); });
    wrap_number_bucket<nb_pu, padded_unique_ptr<nb_pu>>(m, "number_bucket_pu");
    wrap_number_bucket<nb_sh, py::smart_holder>(m, "number_bucket_sh");
#endif
}
