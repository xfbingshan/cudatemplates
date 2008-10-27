/* 
  Cuda Templates.

  Copyright (C) 2008 Institute for Computer Graphics and Vision,
                     Graz University of Technology
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CUDA_STORAGE_H
#define CUDA_STORAGE_H


#include <cudatemplates/layout.hpp>


namespace Cuda {

/**
   Class to represent memory that can be allocated and freed.
*/
template <class Type, unsigned Dim>
class Storage: virtual public Layout<Type, Dim>
{
public:
#ifndef CUDA_NO_DEFAULT_CONSTRUCTORS
  /**
     Default constructor.
  */
  inline Storage()
  {
  }
#endif

  /**
     Constructor.
     @param _size requested size
  */
  inline Storage(const Size<Dim> &_size):
    Layout<Type, Dim>(_size)
  {
  }

  /**
     Constructor.
     @param layout requested layout
  */
  inline Storage(const Layout<Type, Dim> &layout):
    Layout<Type, Dim>(layout)
  {
  }

  /**
     Destructor.
  */
  virtual ~Storage()
  {
  }

  /**
     Allocate memory.
  */
  virtual void alloc() = 0;

  /**
     Allocate memory.
     @param _size requested size
  */
  void alloc(const Size<Dim> &_size);

  /**
     Allocate memory.
     @param layout requested layout
  */
  void alloc(const Layout<Type, Dim> &layout);

  /**
     Free memory.
  */
  virtual void free() = 0;

  /**
     Get current layout.
     @return layout
  */
  const Layout<Type, Dim> &getLayout() const { return *this; }

  /**
     Initialize data structure.
     Subclasses should use this method to initialize their internal data
     representation to a defined "empty" state.
  */
  virtual void init() = 0;

protected:
  /**
     Copy constructor.
  */
  Storage(const Storage &s): Layout<Type, Dim>(s) {}
};

template <class Type, unsigned Dim>
void Storage<Type, Dim>::
alloc(const Size<Dim> &_size)
{
  free();
  this->setSize(_size);
  alloc();
}

template <class Type, unsigned Dim>
void Storage<Type, Dim>::
alloc(const Layout<Type, Dim> &layout)
{
  free();
  this->setLayout(layout);
  alloc();
}

/**
   Class to represent memory that is accessible via a typed pointer.
   Host and device memory are accessible by pointers, but CUDA arrays are not.
*/
template <class Type, unsigned Dim>
class Pointer: virtual public Layout<Type, Dim>
{
public:
#ifndef CUDA_NO_DEFAULT_CONSTRUCTORS
  /**
     Default constructor.
  */
  inline Pointer():
    buffer(0)
  {
  }
#endif

  /**
     Constructor.
     @param _size requested size
  */
  inline Pointer(const Size<Dim> &_size):
    Layout<Type, Dim>(_size),
    buffer(0)
  {
  }

  /**
     Constructor.
     @param layout requested layout
  */
  inline Pointer(const Layout<Type, Dim> &layout):
    Layout<Type, Dim>(layout),
    buffer(0)
  {
  }

  /**
     Destructor.
  */
  virtual ~Pointer()
  {
  }

  /**
     Array index operator.
     @param i index
     @return value at index i
  */
  inline Type &operator[](size_t i) { return buffer[i]; }

  /**
     Array index operator.
     @param i index
     @return value at index i
  */
  inline Type &operator[](const Size<Dim> &i) { return buffer[this->getOffset(i)]; }

  /**
     Array index operator.
     @param i index
     @return value at index i (constant)
  */
  inline const Type &operator[](size_t i) const { return buffer[i]; }

  /**
     Array index operator.
     @param i index
     @return value at index i
  */
  inline const Type &operator[](const Size<Dim> &i) const { return buffer[this->getOffset(i)]; }

  /**
     Get buffer pointer.
     @return buffer pointer (constant)
  */
  inline const Type *getBuffer() const { return buffer; }

  /**
     Get buffer pointer.
     @return buffer pointer
  */
  inline Type *getBuffer() { return buffer; }

protected:
  Type *buffer;

  inline Pointer(const Pointer<Type, Dim> &x):
    Layout<Type, Dim>(x),
    buffer(0)  // subclasses must perform a copy operation
  {
  }
};

/**
   Class to represent memory that can be allocated and freed
   and is accessible via a typed pointer.
*/
template <class Type, unsigned Dim>
class PointerStorage: public Storage<Type, Dim>, virtual public Pointer<Type, Dim>
{
public:
#ifndef CUDA_NO_DEFAULT_CONSTRUCTORS
  /**
     Default constructor.
  */
  inline PointerStorage()
  {
  }
#endif

  /**
     Constructor.
     @param _size requested size
  */
  inline PointerStorage(const Size<Dim> &_size):
    Layout<Type, Dim>(_size),
    Pointer<Type, Dim>(_size),
    Storage<Type, Dim>(_size)
  {
  }

  /**
     Constructor.
     @param layout requested layout
  */
  inline PointerStorage(const Layout<Type, Dim> &layout):
    Layout<Type, Dim>(layout),
    Pointer<Type, Dim>(layout),
    Storage<Type, Dim>(layout)
  {
  }

  /**
     Initialize data structure.
  */
  inline void init() { this->buffer = 0; }

protected:
  inline PointerStorage(const PointerStorage<Type, Dim> &x):
    Layout<Type, Dim>(x),
    Pointer<Type, Dim>(x),
    Storage<Type, Dim>(x)
  {
  }

};

}


#endif
