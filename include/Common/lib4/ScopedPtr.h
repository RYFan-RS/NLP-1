#ifndef __SCOPED_POINTER_H_
#define __SCOPED_POINTER_H_

typedef void var_vd;

template<typename T>
class scoped_ptr
{
public:
    scoped_ptr<T>(T* p = NULL): m_p(NULL)
    {
        m_p = p;
    }
    ~scoped_ptr<T>()
    {
        if (NULL != m_p) delete m_p;
    }
    var_vd attach(T* p)
    {
        if (NULL != p) delete m_p;
        m_p = p;
    }
    var_vd detach()
    {
        m_p = NULL;
    }

private:
    T* m_p;
};

template<typename T>
class scoped_array
{
public:
    scoped_array<T>(T* p): m_p(NULL)
    {
        m_p = p;
    }
    ~scoped_array<T>()
    {
        if (NULL != m_p) delete[] m_p;
    }
    var_vd attach(T* p)
    {
        if (NULL != p) delete[] m_p;
        m_p = p;
    }
    var_vd detach()
    {
        m_p = NULL;
    }

private:
    T* m_p;
};

#endif
