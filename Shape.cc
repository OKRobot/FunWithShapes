/** 
 *  File: Shape.cc
 *  Author: Oliver Krengel
 *  Date: 10/29/2018
 *
 *  This file was created as practice for virtual functions and polymorphism.
 *  Comments throughout the code describe what certain keywords do and why.
 *  The main function demonstrates polymorphism in practice.
 *
 *  Compile with g++ --std=c++11 Shape.cc -o shape
 *  Run with ./shape
 *  Run valgrind to verify there are no memory leaks:
 *  valgrind --leak-check=full ./shape
 */


#include <iostream>
#include <string>
#include <cmath>

using namespace std;

const double PI = 3.14159;
// Note: please excuse the magic numbers

// Base class
class Shape {
  protected:
    string m_name;

    // Do not allow people to create shapes directly
    Shape(string name) :m_name(name) 
    {
    }

  public:
    // Important, in case inherited classes allocate memory AND
    // delete is called on a Shape pointer to an inherited object
    virtual ~Shape()
    {
    }

    // Pure virtual functions, must be defined in inherited classes
    virtual double get_area(void) = 0;
    virtual double get_perimeter(void) = 0;
    string& get_name(void) { return m_name; }

};

class Circle: public Shape {
  public:
    double m_radius;
    double m_area;
    double m_perimeter;
    Circle(string name, double radius) :m_radius(radius), Shape(name)
    {
      m_area = radius*radius*PI; // Could use pow(radius,2)
      m_perimeter = radius*2*PI;
    }

    virtual ~Circle()
    {
    }

    // Overriding get_area and get_perimeter
    // Because of pure virtual function in Shape, this class
    // will not compile without these functions!
    virtual double get_area(void) override { return m_area; }
    virtual double get_perimeter(void) override { return m_perimeter; }
    
};

class Polygon: public Shape {
  protected:
    unsigned int m_sides;
    double m_area;
    double m_perimeter;
  public:
    Polygon(string name, unsigned int sides) :Shape(name), m_sides(sides)
    {
    }
    
    virtual ~Polygon()
    {
    }
    virtual double get_area(void) override { return m_area; }
    virtual double get_perimeter(void) override { return m_perimeter; }
    
};

class Rectangle: public Polygon {
  protected:
    double m_side1;
    double m_side2;
  public:
    Rectangle(string name, double side1, double side2)
    : m_side1(side1), m_side2(side2), Polygon(name, 4)
    {
      m_area = side1*side2;
      m_perimeter = (side1+side2)*2;
    }
    virtual ~Rectangle()
    {
    }
    // Due to this statement, square cannot have its own virtual function
    virtual double get_perimeter(void) override final { return m_perimeter; }

};

class Square: public Rectangle {
  public:
    Square(string name, double side) :Rectangle(name, side, side)
    {
    }

    virtual ~Square()
    {
    }
    // final is called in Rectangle, so Square cannot define get_perimeter
    //virtual double get_perimeter(void) { cout<<"I'm lyin' "; return 0; }

    // You will know if this is called by the exclamation
    // As opposed to Polygon's implementation, which doesn't shout at you
    virtual double get_area(void) override
    { 
      cout<<"The square of "<<m_side1<<"!"<<endl;
      return m_area;
    }

};


class Trapezoid: public Polygon {
  protected:
    double m_long_side;
    double m_short_side;
    double m_height;
    double* m_angled_side_length;
  public:
    Trapezoid(string name, double long_side, double short_side, double height)
    : m_long_side(long_side), m_short_side(short_side), \
      m_height(height), Polygon(name, 4)
    {
      m_area = height*(long_side+short_side)/2;

      // Allocate memory in the constructor
      m_angled_side_length = new double;

      *m_angled_side_length = sqrt(pow((long_side-short_side)/2,2) + \
                                   pow(height,2));
      m_perimeter = short_side+long_side + 2*(*m_angled_side_length);
    }

    // If this is called from a Shape or Polygon pointer, it will
    // still be called if the pointer points to a Trapezoid
    // and we have made our destructors virtual
    virtual ~Trapezoid()
    {
      // Deallocate memory in the destructor
      delete m_angled_side_length;
    }
    
    // No need to define get_area and get_perimeter, they are defined above
    // Since all the functionality is in Polygon, more would be redundant
};

int main()
{
  Circle* c = new Circle("the hole",2);
  // Cannot change s after this, because it is a reference
  Shape &s = *c;
  cout<<"Using Shape reference to Circle object: "<<endl;
  cout<<"The area of "<<s.get_name()<<" is: "<<s.get_area()<<endl;
  cout<<"The perimeter of "<<s.get_name()<<" is: "<<s.get_perimeter()<<endl;
  cout<<endl;
  // Virtul functions override

  // Free memory used for c directly
  delete c;

  Rectangle* r = new Rectangle("the table", 3, 4);
  Shape* s1 = r;

  cout<<"Using Shape pointer to Rectangle object: "<<endl;
  cout<<"The area of "<<s1->get_name()<<" is: "<<s1->get_area()<<endl;
  cout<<"The perimeter of "<<s1->get_name()<<" is: "<<s1->get_perimeter()<<endl;
  cout<<endl;

  // Deleting r or s1 will do the job here
  delete s1;
  // r and s1 can no longer be used, but could be set to something else


  Square* q = new Square("the box", 1);
  Polygon *p = q;

  // Calling from p and q have the same results, because get_area and
  // get_perimeter functions are virtual
  cout<<"Calling from Square object: "<<endl;
  cout<<"The area of "<<q->get_name()<<" is: "<<q->get_area()<<endl;
  cout<<"The perimeter of "<<q->get_name()<<" is: "<<q->get_perimeter()<<endl;
  cout<<endl;

  cout<<"Calling from Polygon pointer to Square object: "<<endl;
  cout<<"The area of "<<p->get_name()<<" is: "<<p->get_area()<<endl;
  cout<<"The perimeter of "<<p->get_name()<<" is: "<<p->get_perimeter()<<endl;

  delete p;
  // p points to deleted memory

  // Allocate memory for t AND allocate memory for it's m_angled_side_length 
  Trapezoid* t = new Trapezoid("the stand", 4, 2, 1);

  // p is still usable after memory has been free'd using its previous value
  p = t;

  cout<<"Calling from Polygon pointer to Trapezoid object: "<<endl;
  cout<<"The area of "<<p->get_name()<<" is: "<<p->get_area()<<endl;
  cout<<"The perimeter of "<<p->get_name()<<" is: "<<p->get_perimeter()<<endl;
  cout<<endl;

  delete p; // Free's allocated memory in Trapezoid because virtual destructors
            // are being used! Otherwise deleting a Polygon pointer would only
            // call the destructor for the base class.

  return 0;
}
