#ifndef __MAPMATRIX_H__
#define __MAPMATRIX_H__

#include <map>
#include <iostream>

namespace colibry {

    class MMOutOfRange {};
    
    template <class I, class T>   // index type, data type
    class MapMatrix {
    public:
	MapMatrix(unsigned int size) : m_rows(size), m_cols(size) {}
	MapMatrix(unsigned int nrows, ncols) : m_rows(nrows), m_cols(ncols) {}
	MapMatrix(const MapMatrix& mm) { *this = mm; }
	virtual MapMatrix<I,T>& operator=(const MapMatrix<I,T>& mm);

	virtual T& operator()(const I& i, const I& j);

	unsigned int rows() { return m_mx.size(); }
	unsigned int cols();

	// Debug
	void print(std::ostream& os);
	
    private:
	unsigned int m_rows, m_cols;
	typedef std::map< I,std::map<I,T> > MapMat;
	MapMat m_mx;
    };

    //
    // IMPLEMENTATION
    //

    template <class I, class T>
    MapMatrix<I,T>& MapMatrix<I,T>::operator=(const MapMatrix<I,T>& mm)
    {
	if (&mm != this)
	    m_mx = mm.m_mx;
	return *this;
    }

    template <class I, class T>
    T& MapMatrix<I,T>::operator()(const I& i, const I& j)
    {
	return m_mx[i][j];
    }
    
}; // end namespace

#endif
