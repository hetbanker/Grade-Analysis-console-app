/*main.cpp*/

//
// <Het Banker>
// U. of Illinois, Chicago
// CS 341, Fall 2019
// Project #03: GradeUtil UI
// This program is C++11 dependent
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;

// includes for gradeutil
#include "gradeutil.h"

College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value, department;

    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);
    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);
        
    //this prints the header
    cout<< "** College of " << college.Name<< ", " << college.Semester << " " << value << " **" << endl;
    
    // second line contains column headers --- skip
    getline(file, line);
        
    //
    // now start inputting and parse course data:
    //

    int totalNumOfStudents=0;
    int totalNumOfCourses =0;
    
    while (getline(file, line))
    {
        Course c = ParseCourse(line);
    
        //
        // search for correct dept to ask course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            //
            // doesn't exist, so we have to create a new dept
            // and insert course:
            //
            Dept d(c.Dept);
            d.Courses.push_back(c);
            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }
            
                totalNumOfStudents += c.getNumStudents();
    } //while
    
        //get the number of courses in the department
        for (auto &i : college.Depts)
        {
            totalNumOfCourses += i.Courses.size();
        }
        cout<<"# of courses taught: " << totalNumOfCourses<<endl;
        cout<<"# of students taught: "<< totalNumOfStudents<<endl;
        
    return college;
}

/**********************************************************************************/
// TODO: define your own functions
//this gives us the grade distribution for department
template <typename T>
void gradeDistribution(const T& department)
{
    GradeStats grade = GetGradeDistribution(department);
    double A = grade.PercentA;
    double B = grade.PercentB;
    double C = grade.PercentC;
    double D = grade.PercentD;
    double F = grade.PercentF;
    cout<<" grade distribution (A-F): " << A << "%, " << B << "%, " << C << "%, " << D << "%, " << F << "%" <<endl;
}

//this gives us the dfw of each class in department
template <typename X>
void dfw(const X& department)
{
    int N=0, DFW=0;
    double dfwForCollege=0;
    dfwForCollege=GetDFWRate(department, N,DFW);
    cout<< " DFW rate: " << dfwForCollege << "%" << endl;
}

//get the grading type
template <typename Y>
void gradingType(const Y& c)
{
    string gradingTypee;
    if(c.getGradingType()==Course::Letter)
    {
            gradingTypee="letter";
    }
    else if(c.getGradingType()==Course::Satisfactory)
    {
            gradingTypee="satisfactory";
    }
    else if(c.getGradingType()==Course::Unknown)
    {
            gradingTypee="unknown";
    }
    cout<<" course type: "<<gradingTypee<<endl;
}

//prints the departmenr info
template <typename Z>
void printInfo(const Z& c)
{
    cout<<c.Dept<<" "<<c.Number<<" (section "<<c.Section<<"): "<<c.Instructor<<endl;
                   cout<<" # students: "<<c.getNumStudents()<<endl;
    
}
/******************************************************************************/
//summary helper function
void summaryHelper(const Dept& department)
{
    //get the number of students and courses in the department
    int totalNumOfStudents=department.NumStudents();
    int totalNumOfCourses= department.NumCourses();
    
    cout<<department.Name<<":"<<endl;
    cout<<" # courses taught: " << totalNumOfCourses<<endl;
    cout<<" # students taught: "<< totalNumOfStudents<<endl;
    
    //calcuate the grade distrubution and its dfw
    gradeDistribution(department);
    dfw(department);
}

void summary(const College& c)
{
    string depName=" ";
    cout<<"dept name, or all? ";
    cin>>depName;

    if(depName != "all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                             {
                                if(x.Name.find(depName)==0) return true;
                                else return false;
                            });
        
        if(X != c.Depts.end())
        {
            summaryHelper(*X);
        }
        else
        {
            cout<<"**dept not found"<<endl;
        }
    }
    
    else if(depName=="all")
    {
        for(auto& i : c.Depts)
        {
            summaryHelper(i);
        }
    }
}
/******************************************************************************/
//search helper
void courseInfoFunc(const vector<Course> &courseInfo)
{
        //if no class the none found
        if(courseInfo.size()==0)
        {
            cout<<"**none found"<<endl;
        }
        else
        {
            //loop to get the info of each class and their data
            for(auto &i : courseInfo)
            {
                printInfo(i);
                gradingType(i);
                gradeDistribution(i);
                dfw(i);
            }
        }
}

//search funciton
void search(const College& c)
{
    string depName=" ";
    string instructorPrefix =" ";
    int courseNumber=0;
    
    cout<<"dept name, or all? ";
    cin>>depName;
    cout<<"course # or instructor prefix? ";
    cin>> instructorPrefix;

    stringstream ss(instructorPrefix);  // create stringstream object
    ss>>courseNumber;   // try to convert input to a course #:
    
    //for each department
    if(depName != "all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                                             {
                                                 if(x.Name.find(depName)==0) return true;
                                                 else return false;
                                             });
        
        if(X != c.Depts.end())
        {
                if(ss.fail())
                {
                    vector<Course> courseInfo=FindCourses(*X,instructorPrefix);
                    courseInfoFunc(courseInfo);
                }
            
                else
                {
                    vector<Course> courseInfo=FindCourses(*X,courseNumber);
                    courseInfoFunc(courseInfo);
                }
        }
        else
        {
            cout<<"**dept not found"<<endl;
        }
    }
    
    //for all the departments in college
    else if(depName == "all")
    {
        if(ss.fail())
                {
                    vector<Course> courseInfo=FindCourses(c,instructorPrefix);
                    courseInfoFunc(courseInfo);
                }
                else
                {
                    vector<Course> courseInfo=FindCourses(c,courseNumber);
                    courseInfoFunc(courseInfo);
                }
        }
}

/************************************************************************************/
void satisfactoryyHelper(const Course& c,vector<Course> &courses)
{
    if(c.getGradingType()==Course::Satisfactory)
    {
        courses.push_back(c);
    }
     std::sort(courses.begin(), courses.end(), [](const Course &x,const Course& y)
     {
         if(x.Dept<y.Dept) return true;
         else if(x.Dept>y.Dept) return false;
         else if(x.Number<y.Number) return true;
         else if(x.Number>y.Number) return false;
         else if(x.Section<y.Section) return true;
         else return false;
     });
}

void satisfactoryy(const College& c)
{
    string depName=" ";
    cout<<"dept name, or all? ";
    cin>>depName;
    string gradingType="";
    vector<Course> courses;
    
    if(depName!="all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                             {
                                if(x.Name.find(depName)==0) return true;
                                else return false;
                            });
        
        if(X != c.Depts.end())
        {
            for(auto &x :(*X).Courses)
            {
                satisfactoryyHelper(x,courses);
            }
        }
        else
        {
            cout<<"**dept not found"<<endl;
        }
    }
    
    else if(depName=="all")
    {
        for(const Dept& dept: c.Depts)
        {
            for(const Course& x: dept.Courses)
            {
                satisfactoryyHelper(x,courses);
            }
        }
    }
       
    if(courses.size()==0)
    {
        cout<<"**none found"<<endl;
    }
    
    for(auto &i : courses)
    {
        printInfo(i);
        
        string gradingType;
        
        if(i.getGradingType()==Course::Letter)
        {
                gradingType="letter";
        }
        else if(i.getGradingType()==Course::Satisfactory)
        {
                gradingType="satisfactory";
        }
        else if(i.getGradingType()==Course::Unknown)
        {
                gradingType="unknown";
        }

        cout<<" course type: "<<gradingType<<endl;
    }
}


/******************************************************************************/
void dfwHelper(const Course& c,vector<Course> &courses,int& N, int& DFW, double& threshold)
{
    if(GetDFWRate(c,DFW,N) > threshold)
    {
        courses.push_back(c);
    }
    std::sort(courses.begin(), courses.end(), [](const Course &x,const Course& y)
    {
        int n,dfw=0;
        double dfwX= GetDFWRate(x,dfw,n);
        double dfwY= GetDFWRate(y,dfw,n);
        
        if (dfwX>dfwY) return true;
        else if(dfwX<dfwY) return false;
        else if(x.Dept<y.Dept) return true;
        else if(x.Dept>y.Dept) return false;
        else if(x.Number<y.Number) return true;
        else if(x.Number>y.Number) return false;
        else if(x.Section<y.Section) return true;
        else return false;
    });
    
}
void dfw(const College& c)
{
    string depName=" ";
    cout<<"dept name, or all? ";
    cin>>depName;
    cout<<"dfw threshold? ";
    double threshold=0;
    int N,DFW=0;
    cin>>threshold;
    vector <Course> courses;
    
    if(depName!="all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                {
                    if(x.Name.find(depName)==0) return true;
                    else return false;
                });
        
        if(X != c.Depts.end())
        {
            for(auto &x :(*X).Courses)
            {
                dfwHelper(x,courses,N,DFW,threshold);
            }
        }
        else
        {
             cout<<"**dept not found"<<endl;
             return;
        }
    }
    
    else if(depName=="all")
    {
       for(const Dept& dept: c.Depts)
        {
            for(const Course& x: dept.Courses)
            {
                dfwHelper(x,courses,N,DFW,threshold);
            }
        }
    }
    
    if(courses.size()==0)
    {
        cout<<"**none found"<<endl;
    }
    
    //loop to print the info for each course
    for(auto &i:courses)
    {
        printInfo(i);
        gradingType(i);
        gradeDistribution(i);
        dfw(i);
    }
}
/******************************************************************************/
void letterBHelper(const Course& c,vector<Course> &courses, double& threshold)
{
    GradeStats grade = GetGradeDistribution(c);
    if(grade.PercentB > threshold)
    {
        courses.push_back(c);
    }
    std::sort(courses.begin(), courses.end(), [](const Course &x,const Course& y)
    {

        GradeStats letterBX= GetGradeDistribution(x);
        GradeStats letterBY= GetGradeDistribution(y);
        
        if (letterBX.PercentB>letterBY.PercentB) return true;
        else if(letterBX.PercentB<letterBY.PercentB) return false;
        else if(x.Dept<y.Dept) return true;
        else if(x.Dept>y.Dept) return false;
        else if(x.Number<y.Number) return true;
        else if(x.Number>y.Number) return false;
        else if(x.Section<y.Section) return true;
        else return false;
    });
    
}
void letterB(const College &c)
{
    string depName=" ";
    cout<<"dept name, or all? ";
    cin>>depName;
    cout<<"letter B threshold? ";
    double threshold=0;
    cin>>threshold;
    vector <Course> courses;
    
    if(depName!="all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                                             {
                                                 if(x.Name.find(depName)==0) return true;
                                                 else return false;
                                             });
        
        if(X != c.Depts.end())
        {
            for(auto &x :(*X).Courses)
            {
                letterBHelper(x,courses,threshold);
            }
        }
        else
        {
             cout<<"**dept not found"<<endl;
             return;
        }
    }
    
    else if(depName=="all")
    {
       for(const Dept& dept: c.Depts)
        {
            for(const Course& x: dept.Courses)
            {
                letterBHelper(x,courses,threshold);
            }
        }
    }
    
    if(courses.size()==0)
    {
        cout<<"**none found"<<endl;
    }
    
    for(auto &i:courses)
    {
        printInfo(i);
        gradingType(i);
        gradeDistribution(i);
        dfw(i);
    }
}

/*******************************************************************************************/
void average(const College &c, const Course &c1)
{
    string depName=" ";
    cout<<"dept name, or all? ";
    cin>>depName;
    vector <Course> courses;
    vector <Dept> depts;
    
    vector<pair<string,double>> department;
    vector<pair<Course,double>> course ;
    
    if (depName == "all")
    {
        for(const Dept& dept: c.Depts)
        {
            for(const Course& x: dept.Courses)
            {
                courses.push_back(x);
            }
        }
        for(const Dept& x : c.Depts)
        {
            int numCourse=0;
            double averageClass=0.0;
            double averageDept=0.0;
            for (const Course& i : x.Courses)
            {
                if(i.getGradingType()!=Course::Letter) continue;
                GradeStats grade = GetGradeDistribution(i);
                double A = grade.NumA;
                double B = grade.NumB;
                double C = grade.NumC;
                double D = grade.NumD;
                double F = grade.NumF;
               
                double cheker= A+B+C+D+F;
                double GPA= (double)(A*4 + B*3 + C*2 +D*1 + F*0)/ (double)(A+B+C+D+F);
                
                if(cheker==0){ GPA=0.0;}
                averageClass += GPA;
                numCourse++;
            }
           
            if(numCourse==0)
            {
                averageDept=0.0;
            }
            else
            {
                averageDept = (double)averageClass/(double)numCourse;
            }
            
            auto keyvaluepair = std::pair<string,double>(x.Name,averageDept);
            if(numCourse != 0)
            {
                department.push_back(keyvaluepair);
            }
        }
        
        sort(department.begin(), department.end(), [=](std::pair<string, double>& a, std::pair<string, double>& b)
        {
            return a.second > b.second;
        });
        
        for (auto itr = department.begin(); itr != department.end(); ++itr)
        {
            cout <<"Overall GPA for "<< itr->first <<" : " << itr->second<<endl;
        }
    }
    ////////////
    else if(depName!="all")
    {
        auto X= std::find_if(c.Depts.begin(), c.Depts.end(),[=](const Dept& x)
                    {
                        if(x.Name.find(depName)==0) return true;
                        else return false;
                    });
        
        if(X != c.Depts.end())
        {
            for(auto &x :(*X).Courses)
            {
                courses.push_back(x);
            }

            for (const Course& i : courses)
            {
                int numCourse=0;
                double averageClass=0.0;
                if(i.getGradingType()!=Course::Letter) continue;
                GradeStats grade = GetGradeDistribution(i);
                double A = grade.NumA;
                double B = grade.NumB;
                double C = grade.NumC;
                double D = grade.NumD;
                double F = grade.NumF;
                    
                double cheker= A+B+C+D+F;
                double GPA= (double)(A*4 + B*3 + C*2 +D*1 + F*0)/ (double)(A+B+C+D+F);
                        
                if(cheker==0)
                {
                    GPA=0.0;
                }
                averageClass += GPA;
                numCourse++;
                auto keyvaluepair = std::pair<Course,double>(i,averageClass);
                course.push_back(keyvaluepair);
                        
            }
    
            sort(course.begin(), course.end(), [=](std::pair<Course, double>& a, std::pair<Course, double>& b)
            {
                if(a.second > b.second)return true;
                else if(a.second < b.second)return false;
                else if(a.first.Number < b.first.Number) return true;
                else if(a.first.Number > b.first.Number) return false;
                else if(a.first.Section < b.first.Section) return true;
                else return false;
            
            });
            
            for (auto itr = course.begin(); itr != course.end(); ++itr)
            {
                cout <<"Overall GPA for "<< itr->first.Dept<<" "<< itr->first.Number<<"("<<itr->first.Section<<")"<<" : " << itr->second<<endl;
            }
        }
        else
        {
             cout<<"**dept not found"<<endl;
             return;
        }
    }
}
void commands(const College& college, string& command, const Course& course)
{
    while(command != "#")
    {
        if(command == "summary")
        {
            summary(college);
        }
        else if(command =="search")
        {
            search(college);
        }
        else if(command=="satisfactory")
        {
            satisfactoryy(college);
        }
        else if(command=="dfw")
        {
            dfw(college);
        }
        else if(command=="letterB")
        {
            letterB(college);
        }
        else if(command=="average")
        {
            average(college,course);
        }
        else
        {
            cout<<"**unknown command"<<endl;
        }
        cout<<"Enter a command> ";
        cin>>command;
    }
}

//this function sort departments
void sortDept(College &college)
{
     std::sort(college.Depts.begin(), college.Depts.end(),[](const Dept& x, const Dept& y)
       {
         if(x.Name<y.Name) return true;
         else return false;
       });
}
/******************************************************************************/
int main()
{
    string filename;

    cout << std::fixed;
    cout << std::setprecision(2);

    cin >> filename;
    College college = InputGradeData(filename);
    Course c1;
    GradeStats grade = GetGradeDistribution(college);

    cout<<"grade distribution (A-F): " << grade.PercentA << "%, " << grade.PercentB << "%, " << grade.PercentC << "%, " << grade.PercentD << "%, " << grade.PercentF << "%" <<endl;
    
    //calculating the dfw rate
    int N=0, DFW=0;
    cout<< "DFW rate: " << GetDFWRate(college, N,DFW) << "%" << endl;
    
    //this sorts the department
    sortDept(college);
    
    //for commands
    string command;
    cout<<"\nEnter a command> ";
    cin>>command;
    commands(college,command,c1); //functions for commands
    
    return 0;
}
