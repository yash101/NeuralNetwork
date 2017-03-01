#ifndef MATRIX_H
#define MATRIX_H

#include "core.h"

#include <new>
#include <string.h>
#include <stdexcept>

namespace nn
{
  template<typename T>
  class Matrix
  {
  private:
    T** _elems;
    long* _refct;
    long* _width;
    long* _height;

    /*
     * Matrix<T>::construct
     * - Description: Initializes all variables to a state to reduce bugs; sets pointers to nullptr to prevent accidental behaviour
     * - Parameters
     *   - void
     */
    void construct()
    {
      _elems = nullptr;
      _refct = nullptr;
      _width = nullptr;
      _height = nullptr;
    }

    /*
     * Matrix<T>::gc
     * - Desctiption: Reduces the global reference count; if global reference count is less than or equal to zero, all data is deleted
     * - Parameters
     *   - void
     */
    bool gc()
    {
      if(!_refct) return false;
      if(*_refct-- <= 0)
      {
        if(_elems)
        {
    if(*_elems)
    {
      delete[] *_elems;
    }
    delete _elems;
        }
        delete _width;
        delete _height;
      }

      return true;
    }
  public:

    /*
     * Matrix<T>::create_local
     * - Description: Creates a new matrix, and unlinks this object from any other Matrix's. Performs GC, thus "unlinking" this object from any others
     * - Parameters
     *   - [long] width: Width of the matrix to be created
     *   - [long] height: Height of the matrix to be created
     */
    bool create_local(long width, long height)
    {
      gc();
      _refct = new (std::nothrow) decltype(*_refct);
      _width = new (std::nothrow) decltype(*_width);
      _height = new (std::nothrow) decltype(*_height);
      _elems = new (std::nothrow) decltype(*_elems);
      if(!_refct || !_width || !_height || !_elems)
      {
  #define deleteif(x) if(!x) delete x
        deleteif(_refct);
        deleteif(_width);
        deleteif(_height);
        deleteif(_elems);
  #undef delete
        return false;
      }

      *_refct = 1;
      *_width = width;
      *_height = height;
      *_elems = new T[(*_width) * (*_height)];

      if(!*_elems)
      {
        delete _elems;
        delete _width;
        delete _height;
        delete _refct;

        return false;
      }

      return true;
    }

    /*
     * Matrix<T>::create_global
     * - Description: Creates a new matrix; updates the data in all connected objects
     * - Parameters
     *   - [long] width: Width of the matrix to be created
     *   - [long] height: Height of the matrix to be created
     */
    bool create_global(long width, long height)
    {
      if(!_refct)
      {
        return create_local(width, height);
      }
      else
      {
        T* arr = new (std::nothrow) T[width * height];
        if(!arr) return false;

        delete *_elems;
        *_elems = arr;
        *_width = width;
        *_height = height;
      }
      return true;
    }

    /*
     * Matrix<T>::get
     * - Description: retrieves a point in the matrix; checks bounds
     * - Parameters
     *   - [long] x: X-position of the element to retrieve
     *   - [long] y: Y-position of the element to retrieve
     */
    inline T& get(long x, long y)
    {
      if(!_elems || !*_elems || x >= *_width || y >= *_height || x < 0 || y < 0)
        throw std::out_of_range("Error: Matrix<T>::get out of range");
      return (*_elems)[(*_width) * x + y];
    }

    /*
     * Matrix<T>::get_unchecked
     * - Description: retrieves a point in the matrix; does NOT check bounds
     * - Parameters
     *   - [long] x: X-position of the element to retrieve
     *   - [long] y: Y-position of the element to retrieve
     */
    inline T& get_unchecked(long x, long y)
    {
      return (*_elems)[(*_width) * x + y];
    }

    /*
     * Matrix<T>::clone
     * - Description: copies the matrix and then returns the copied version
     * - Parameters
     *   - void
     */
    Matrix<T> clone()
    {
      Matrix<T> mat(_width, _height);
      memcpy(*mat._elems, *_elems, (*_width) * (*_height) * sizeof(T));
      return mat;
    }

    /*
     * Matrix<T>::Matrix
     * - Description: constructs the object
     * - Parameters
     *   - void
     */
    Matrix()
    {
      construct();
    }

    /*
     *  Matrix<T>::Matrix
     * - Description: constructs the object with a width and height
     * - Parameters
     *   - [long] width: Width of the matrix to be created
     *   - [long] height: Height of the matrix to be created
     */
    Matrix(long width, long height)
    {
      construct();
      create_global(width, height);
    }

    /*
     * Matrix<T>::Matrix
     * - Description: constructs the object with another matrix object
     * - Parameters
     *   - [Matrix<T>] mat: A matrix to copy the reference from and set GC
     */
    Matrix(const Matrix<T>& mat)
    {
      construct();
      _elems = mat._elems;
      _width = mat._width;
      _height = mat._height;
      _refct = mat._refct;

      (*_refct)++;
    }

    /*
     * Matrix<T>::operator=
     * - Description: Sets the matrix equal to another matrix
     * - Parameters
     *   - [Matrix<T>&] m: A matrix to copy the reference from and set GC
     */
    Matrix& operator=(const Matrix<T>& m)
    {
      gc();
      construct();
      _elems = m._elems;
      _width = m._width;
      _height = m._height;
      _refct = m._refct;
      (*_refct)++;

      return *this;
    }

    /*
     * Matrix<T>::~Matrix()
     * - Description: destroys the object, performing reference counting based GC
     * - Parameters:
     *   - void
     */
    virtual ~Matrix()
    {
      gc();
    }
  };
}
#endif // MATRIX_H
