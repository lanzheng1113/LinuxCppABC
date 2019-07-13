#pragma once
#include "util/NonCopyable.h"

/**
 * \class DataArrayDeleteAgent
 * DataArrayDeleteAgent 用于在未使用boost::scope_array的地方代替它使用。
 *
 * 作用是用于释放一块内存。当这个类的对象离开作用域后自动释放它内部的指针指向的内存块。
 * 注意仅用于申请内存数组时使用。
 * 这里使用template，仅仅用于方便调试的时候知道是哪一种类型。
 */
namespace qcutil {

    template <class T>
    class DataArrayDeleteAgent : public NonCopyable {
    public:

        DataArrayDeleteAgent(T* ptr) {
            m_ptr = ptr;
        }

        ~DataArrayDeleteAgent() {
            //psv Modify
            //		if (m_ptr)
            {
                delete[] m_ptr;
            }
        }
    protected:
    private:
        T* m_ptr;
    };
}
