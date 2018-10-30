/** 
 *  File: Shape_RAII.cc
 *  Author: Oliver Krengel
 *  Date: 10/29/2018
 *
 *  This file was created as practice for good RAII practices, modified 
 *  from the file Shape.cc.
 *  Comments throughout the code describe what certain keywords do and why.
 *  The main function demonstrates polymorphism and RAII in practice.
 *
 *  Compile with g++ --std=c++11 Shape_RAII.cc -o shape
 *  Run with ./safe
 *  Run valgrind to verify there are no memory leaks:
 *  valgrind --leak-check=full ./safe
 *
 *  This file is free of memory leaks, has no calls to delete!
 */


#include <iostream>
#include <string>
#include <cmath>
#include <memory>

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
  //Circle* c = new Circle("the hole",2);
  // Cannot change s after this, because it is a reference

  // Create shared pointer to c
  shared_ptr<Circle> c = make_shared<Circle>("the hole", 2);
  // Note that the dereferenced shared_ptr has the same type as the original
  Shape &s = *c;
  // So our s reference need not change
  cout<<"Using Shape reference to Circle object: "<<endl;
  cout<<"The area of "<<s.get_name()<<" is: "<<s.get_area()<<endl;
  cout<<"The perimeter of "<<s.get_name()<<" is: "<<s.get_perimeter()<<endl;
  cout<<endl;
  // Virtul functions override

  // Free memory used for c directly
  //delete c;
  // No need for delete, c will be deleted when it goes out of scope

  //Rectangle* r = new Rectangle("the table", 3, 4);
  unique_ptr<Rectangle> r1 = make_unique<Rectangle>("the table", 3, 4);
  
  // This will not compile, since r1 is unique
  //Shape* s1 = r1;

  // It is okay to leave r1 as-is, though it will be unused, because it
  // will be deleted when it goes out of scope
  shared_ptr<Rectangle> r = make_shared<Rectangle>("the table", 4, 5);

  // Unlike the reference case, we cannot assign a pointer to a shared_ptr
  // Shape* s1 = r;
  shared_ptr<Shape> s1 = r;

  cout<<"Using Shape pointer to Rectangle object: "<<endl;
  cout<<"The area of "<<s1->get_name()<<" is: "<<s1->get_area()<<endl;
  cout<<"The perimeter of "<<s1->get_name()<<" is: "<<s1->get_perimeter()<<endl;
  cout<<endl;

  // no longer need to delete s1
  //delete s1;
  // r and s1 can no longer be used, but could be set to something else


  auto q = make_shared<Square>("the box", 1);
  // Using auto here would make the pointer a shared_ptr<Square>
  // auto p = q;
  // We want a shared_ptr<Polygon>
  shared_ptr<Polygon> p = q;

  // Calling from p and q have the same results, because get_area and
  // get_perimeter functions are virtual
  cout<<"Calling from Square object: "<<endl;
  cout<<"The area of "<<q->get_name()<<" is: "<<q->get_area()<<endl;
  cout<<"The perimeter of "<<q->get_name()<<" is: "<<q->get_perimeter()<<endl;
  cout<<endl;

  cout<<"Calling from Polygon pointer to Square object: "<<endl;
  cout<<"The area of "<<p->get_name()<<" is: "<<p->get_area()<<endl;
  cout<<"The perimeter of "<<p->get_name()<<" is: "<<p->get_perimeter()<<endl;

  // No longer need to delete p!
  //delete p;
  // p points to deleted memory

  // Allocate memory for t AND allocate memory for it's m_angled_side_length 
  shared_ptr<Trapezoid> t = make_shared<Trapezoid>("the stand", 4, 2, 1);

  // p is still usable after memory has been free'd using its previous value
  p = t;

  cout<<"Calling from Polygon pointer to Trapezoid object: "<<endl;
  cout<<"The area of "<<p->get_name()<<" is: "<<p->get_area()<<endl;
  cout<<"The perimeter of "<<p->get_name()<<" is: "<<p->get_perimeter()<<endl;
  cout<<endl;

  //delete p; 
  // Free's allocated memory in Trapezoid because virtual destructors
  // are being used! Otherwise deleting a Polygon pointer would only
  // call the destructor for the base class.

  return 0;
}
