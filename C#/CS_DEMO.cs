using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace CS_DEMO
{
    //BASE CLASS (Employee).  This class contains basic information about an employee
    class Employee
    {
        protected string name;
        protected string title;
        protected int empid;
        
        //Set Employee Name
        public void setName (string name)
        {
            this.name = name;
        }

        //Get Employee Name
        public string getName()
        {
            return name;
        }

        //Set Employee Title
        public void setTitle(string title)
        {
            this.title = title;
        }

        //Get Employee Title
        public string getTitle()
        {
            return title;
        }

        //Set Employee ID
        public void setEmpId(int empid)
        {
            this.empid = empid;
        }

        //Get Employee ID
        public int getEmpId()
        {
            return empid;
        }
    }

    //DERIVED CLASS (Programmer).  Contains information on the languages known
    class Programmer: Employee
    {
        //List of programming languages
        private List<string> languages;

        //Constructor - set name,title,employee id, and list of languages
        public Programmer(string name, string title, int empid, List<string> languages, Boss b)
        {
            this.name = name;
            this.title = title;
            this.empid = empid;
            this.languages = languages;
            b.setNumEmployees(b.getNumEmployees() + 1);
        }

        //Get list of languages
        public List<string> getLanguages()
        {
            return languages;
        }

        //Add a new language to the list, only if it doesn't exist already
        public void addLanguage(string language)
        {
            if (!languages.Contains(language))
            {
                System.Console.Write("     Attempting to add language (" + language + ") for " + getName() + " ... ");
                languages.Add(language);
                System.Console.WriteLine("Success");
            }
            else
                System.Console.WriteLine("     Warning: " + getName() + " already knows " + language + "!");
        }
    }

    //DERIVED CLASS (Boss).  Contains information on the number of employees under him.
    class Boss: Employee
    {
        //Number of employees under this boss
        private int num_employees;
        
        //Constructor - set name, title, and employee id
        public Boss(string name, string title, int empid)
        {
            this.name = name;
            this.title = title;
            this.empid = empid;
            num_employees = 0;
        }
        
        //Set number of employees
        public void setNumEmployees(int num_employees)
        {
            this.num_employees = num_employees;
        }

        //Get number of employees
        public int getNumEmployees()
        {
            return num_employees;
        }
    }

    //Main Class for running program
    class WorkTest
    {
        //Create a boss and a set of employees, while altering and printing out information
        static void Main(string[] args)
        {
            //Create our employees
            Boss b = new Boss("Shannon Duncan", "Manager", 1000);
            Programmer p1 = new Programmer("Christopher Rhodes", "Programmer II", 2000, new List<string> { "C++", "Python", "SQL" }, b);
            Programmer p2 = new Programmer("Kyle Hanson", "Programmer I", 2001, new List<string> { "C++", "Python", "SQL" }, b);
            Programmer p3 = new Programmer("Jacob Wright", "Programmer III", 2002, new List<string> { ".NET", "C#", "HTML" }, b);
            Programmer p4 = new Programmer("Allison White", "Programmer I", 2003, new List<string> { "C#", "Perl", "Java" }, b);
            Programmer p5 = new Programmer("Kelley Lawrence", "Programmer II", 2004, new List<string> { "Lisp", "Basic"}, b);

            //Add to generic base list
            List<Employee> emps = new List<Employee>();
            emps.Add(b);
            emps.Add(p1);
            emps.Add(p2);
            emps.Add(p3);
            emps.Add(p4);
            emps.Add(p5);

            //For each employee, do specific tasks determined by Class Type
            foreach (var e in emps)
            {
                //Pull out classtype of object, ignore namespace
                var pattern = @"\.([A-z_]+)";
                var match = Regex.Match(e.GetType().ToString(), pattern);
                string classtype = match.Groups[1].ToString();

                //For Bosses, print name, title and number of employees
                //For Programmers, print name, title, and languages (also alter a specific employee by id)
                switch (classtype)
                {
                    case "Boss":
                        {
                            Boss boss = (Boss)e;
                            System.Console.WriteLine(boss.getName() + "(" + boss.getTitle() + ")");
                            System.Console.WriteLine(boss.getName() + " has " + boss.getNumEmployees() + " employees!\n");
                            break;
                        }
                    case "Programmer":
                        Programmer p = (Programmer)e;
                        System.Console.WriteLine(p.getName() + "(" + p.getTitle() + " - " + String.Join(",", p.getLanguages()) + ")\n");
                        
                        if (p.getEmpId() == 2000)
                        {
                            p.addLanguage("C#");
                            p.addLanguage("C#"); //this is on purpose
                            System.Console.WriteLine("     New Language List for " + p.getName() + " = (" + String.Join(",", p.getLanguages()) + ")\n");
                        }
                        break;
                    default:
                        System.Console.WriteLine("ERROR: New Class Type\n");
                        break;
                }
            } // end for-each
        } // end main
    } // end class WorkTest
} // end namespace