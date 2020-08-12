#include<iostream>
#include<sstream>
#include<vector>
#include<string>
#include<fstream>
#include<algorithm>
using namespace std;
#define MAX_GRADE 20
#define MIN_GRADE 10
#define MIN_GPA_FOR_MAX_UNITS 17
#define TOP_MAX_UNITS 24
#define NORMAL_MAX_UNITS 20
#define MIN_UNITS 12
typedef vector<int> Prerequisites;
struct Schedule{
    vector<string> days;
    vector<int> begin_hours;
    vector<int> end_hours;
};
struct Grade{
    int id;
    float grade_no;
};
struct Course{
    int id;
    string name;
    int units;
    Schedule schedule;
    Prerequisites prerequisites;
};
typedef vector<Course> course_table;
typedef vector<Grade> grade_table;
void read_schedule_section(Course &new_course,const string &schedule){
    string schedule_info_section;
    stringstream schedule_infos,schedule_info,hour_info;
    schedule_infos<<schedule<<endl;
    for(int i=0;i<=count(schedule.begin(),schedule.end(),'/');i++){
        getline(schedule_infos,schedule_info_section,'/');
        schedule_info<<schedule_info_section<<endl;
        getline(schedule_info,schedule_info_section,'-');
        new_course.schedule.days.push_back(schedule_info_section);
        getline(schedule_info,schedule_info_section,'-');
        hour_info<<schedule_info_section<<endl;
        getline(hour_info,schedule_info_section,':');
        new_course.schedule.begin_hours.push_back(stoi(schedule_info_section)*100);
        getline(hour_info,schedule_info_section);
        new_course.schedule.begin_hours[i]+=stoi(schedule_info_section);
        getline(schedule_info,schedule_info_section);
        hour_info<<schedule_info_section<<endl;
        getline(hour_info,schedule_info_section,':');
        new_course.schedule.end_hours.push_back(stoi(schedule_info_section)*100);
        getline(hour_info,schedule_info_section);
        new_course.schedule.end_hours[i]+=stoi(schedule_info_section);
    }
}
void read_prerequisites_section(Course &new_course,string &prerequisite){
    string prerequisite_info_section;
    stringstream prerequisite_info;
    prerequisite_info<<prerequisite<<endl;
    for(int i=0;i<=count(prerequisite.begin(),prerequisite.end(),'-');i++){
        getline(prerequisite_info,prerequisite_info_section,'-');
        new_course.prerequisites.push_back(stoi(prerequisite_info_section));
    }
}
course_table read_course_info(string courses_info_file_name){
    ifstream courses(courses_info_file_name);
    string course_info,temperory,course_info_section;
    stringstream sections;
    course_table courses_info;
    getline(courses,temperory);
    while(getline(courses,course_info)){
        Course new_course;
        sections<<course_info<<endl;
        getline(sections,course_info_section,',');
        new_course.id=stoi(course_info_section);
        getline(sections,course_info_section,',');
        new_course.name=course_info_section;
        getline(sections,course_info_section,',');
        new_course.units=stoi(course_info_section);
        getline(sections,course_info_section,',');
        read_schedule_section(new_course,course_info_section);
        getline(sections,course_info_section);
        read_prerequisites_section(new_course,course_info_section);
        courses_info.push_back(new_course);
    }
    courses.close();
    return courses_info;
}
int find_course(const grade_table &grade_info,int chosen_course_id){
    if(chosen_course_id==0){
        return -2;
    }
    for(int i=0;i<grade_info.size();i++){
        if(grade_info[i].id==chosen_course_id){
            return i;
        }
    }
    return -1;
}
grade_table read_grade_info(string grades_info_file_name){
    ifstream grades(grades_info_file_name);
    string grade_info,temperory,grade_info_section;
    stringstream sections;
    grade_table grades_info;
    getline(grades,temperory);
    int index;
    while(getline(grades,grade_info)){
        Grade new_grade;
        sections<<grade_info<<endl;
        getline(sections,grade_info_section,',');
        new_grade.id=stoi(grade_info_section);
        getline(sections,grade_info_section);
        new_grade.grade_no=stoi(grade_info_section);
        if((index=find_course(grades_info,new_grade.id))>=0){
            grades_info[index].grade_no=new_grade.grade_no;
        }
        else{
            grades_info.push_back(new_grade);
        }
    }
    return grades_info;
}
int acceptable_prerequisites_per_courses(const Course &chosen_course,const grade_table &grades_info){
    int index;
    for(int i=0;i<chosen_course.prerequisites.size();i++){
        index=find_course(grades_info,chosen_course.prerequisites[i]);
        if(index==-1){
            return 0;
        }
        if(index>=0 && grades_info[index].grade_no<MIN_GRADE){
            return 0;
        }
    }
    return 1;
}
course_table find_tackable_courses(const course_table &courses_info,const grade_table &grades_info){
    course_table tackable_courses;
    int index;
    for(int i=0;i<courses_info.size();i++){
        index=find_course(grades_info,courses_info[i].id);
        if(index==-1){
            if(acceptable_prerequisites_per_courses(courses_info[i],grades_info)==1){
                tackable_courses.push_back(courses_info[i]);            
            }
        }
        else if(index>=0 && grades_info[index].grade_no<MIN_GRADE){
            tackable_courses.push_back(courses_info[i]);
        }
    }
    return tackable_courses;
}
int compute_total_units(const course_table &courses_info,const grade_table &grades_info){
    int total_units=0,index=0;
    for(int i=0;i<courses_info.size();i++){
        index=find_course(grades_info,courses_info[i].id);
        if(index>=0){
            total_units+=courses_info[i].units;
        }
    }
    return total_units;
}
float compute_total_grades(const course_table &courses_info,const grade_table &grades_info){
    int index=0;
    float total_grades=0;
    for(int i=0;i<courses_info.size();i++){
        index=find_course(grades_info,courses_info[i].id);
        if(index>=0){
            total_grades+=grades_info[index].grade_no*courses_info[i].units;
        }
    }
    return total_grades;
}
float compute_gpa(const course_table &courses_info,const grade_table & grades_info){
    return compute_total_grades(courses_info,grades_info)/compute_total_units(courses_info,grades_info);
}
int max_units_allowed(float gpa){
    if(gpa>=MIN_GPA_FOR_MAX_UNITS){
        return TOP_MAX_UNITS;
    }
    return NORMAL_MAX_UNITS;
}
int check_interference(const Course &first,const Course &second){
    for(int i=0;i<first.schedule.days.size();i++){
        for(int j=0;j<second.schedule.days.size();j++){
            if(first.schedule.days[i]==second.schedule.days[j]){
                if(first.schedule.begin_hours[i]<second.schedule.end_hours[j] && first.schedule.end_hours[i]>second.schedule.begin_hours[j]){
                    return 1;
                }
            }
        }
    }
    return 0;
}
course_table find_recommended_courses(const course_table &tackable_courses,float gpa){
    course_table recommended_courses;
    int units_taken=0,interference=0,max_units=max_units_allowed(gpa);
    for(int i=0;i<tackable_courses.size();i++){
        for(int j=0;j<recommended_courses.size();j++){
            if(check_interference(tackable_courses[i],recommended_courses[j])==1){
                interference=1;
                break;
            }
        }
        if(interference!=1 && units_taken+tackable_courses[i].units<=max_units){
            recommended_courses.push_back(tackable_courses[i]);
            units_taken+=tackable_courses[i].units;
        }
        interference=0;
    }
    return recommended_courses;
}
bool compare_alphabet(const Course &first,const Course &second){
    return first.name<second.name;
}
bool compare_units_or_alphabet(const Course &first,const Course &second){
    if(first.units!=second.units){
        return first.units>second.units;
    }
    return first.name<second.name;
}
void final_sort_courses(course_table &courses_info){
    sort(courses_info.begin(),courses_info.end(),compare_alphabet);
}
void first_sort_courses(course_table &courses_info){
    sort(courses_info.begin(),courses_info.end(),compare_units_or_alphabet);
}
int main(int argc,char* argv[]){
    course_table courses=read_course_info(argv[1]);
    grade_table grades=read_grade_info(argv[2]);
    float gpa=compute_gpa(courses,grades);
    course_table tackable_courses=find_tackable_courses(courses,grades);
    first_sort_courses(tackable_courses);
    course_table recommended_courses=find_recommended_courses(tackable_courses,gpa);
    final_sort_courses(recommended_courses);
    for(int i=0;i<recommended_courses.size();i++){
        cout<<recommended_courses[i].id<<endl;
    }
    return 0;
}