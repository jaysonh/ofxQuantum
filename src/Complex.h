/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Complex.h
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  This class provides functionality to do mathematical functions on complex numbers
//  Code is based on work done by Matthew Hayward https://quantum-algorithms.herokuapp.com/
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef COMPLEX_H
#define COMPLEX_H

class Complex
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////
    // Public Functions
    //////////////////////////////////////////////////////////////////////////////////////////
    
    // Constructors
    Complex();
    Complex( const double real, const double imag );
    ~Complex();
    
    // Getter/Setter Methods
    void   set( const double new_real, const double new_imaginary);
    double getReal() const;
    double getImag() const;
    
    // Operators
    Complex & operator +  ( const Complex & );
    Complex & operator *  ( const Complex & );
    Complex & operator =  ( const Complex & );
    bool operator      == ( const Complex & ) const;

    // Get the length of this complex number
    double length();

    // Multiply by a float
    Complex & mult( const float & c );
        
private:

    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Variables
    //////////////////////////////////////////////////////////////////////////////////////////
   
    double mReal;
    double mImag;

};

#endif
