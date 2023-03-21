#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <unordered_map>
using namespace std;

// 定义工作台的结构体
struct Workbench {
    int id;             // 工作台编号
    double x, y;        // 工作台坐标
    int remain_time;    // 剩余生产时间
    int type;           //工作台类型
    int material_status;// 原材料格状态
    int product_status; // 产品格状态
};

// 定义机器人的结构体
struct Robot {
    int id;                       // 机器人id
    int workbench_id;             // 所处工作台id
    int carry_item_type;          // 携带物品类型
    double time_value_factor;     // 时间价值系数
    double collision_value_factor;// 碰撞价值系数
    double angular_velocity;      // 角速度
    double velocity_x, velocity_y;// 线速度
    double direction;             // 朝向
    double x, y;                  // 坐标
};
//定义向量
struct Vector {
    double x;
    double y;
};

vector<Workbench> workbenches;
vector<Robot> robots;

//获取单位向量
Vector get_unit_vector(double x, double y) {
    double length = sqrt(x * x + y * y);
    double x_projection = x / length;
    Vector unit_vector = {x_projection, 0};
    return unit_vector;
}
// 解析一行字符串为一个工作台结构体
Workbench parse_workbench(string &str,int i) {
    Workbench wb;
    wb.id = i;
    sscanf(str.c_str(), "%d %lf %lf %d %d %d",
           &wb.type, &wb.x, &wb.y, &wb.remain_time, &wb.material_status, &wb.product_status);
    return wb;
}

// 解析一行字符串为一个机器人结构体
Robot parse_robot(string &str,int i) {
    Robot robot;
    robot.id = i;
    sscanf(str.c_str(), "%d %d %lf %lf %lf %lf %lf %lf %lf %lf",
           &robot.workbench_id, &robot.carry_item_type, &robot.time_value_factor, &robot.collision_value_factor,
           &robot.angular_velocity, &robot.velocity_x, &robot.velocity_y, &robot.direction, &robot.x, &robot.y);
    return robot;
}
//向量点积
double dot_product(const Vector A,const Vector B){
    double dot_product = A.x * B.x + A.y * B.y;
    return dot_product;
}
//向量的模
double vector_magnitude(const Vector A){
     double sum_of_squares = 0;
     sum_of_squares += A.x * A.x;
     return sqrt(sum_of_squares);
}
//两点之间的距离
double get_distance(const Robot robot,const Workbench workbench){
    double distance = (robot.x - workbench.x)*(robot.x - workbench.x) + (robot.y - workbench.y)*(robot.y - workbench.y);
    return distance;
}
//获取航向角
double get_direction(const Robot robot,const Workbench workbench){
    Vector A; //机器人与目标工作台之间的向量
    Vector robot_unit_vec; //机器人所在位置的x轴正方向的单位向量
    
    robot_unit_vec = get_unit_vector(robot.x,robot.y);
    A.x = robot.x - workbench.x;
    A.y = robot.y - workbench.y;
    double beta_angle = acos(dot_product(robot_unit_vec,A)/(vector_magnitude(A)*vector_magnitude(robot_unit_vec)));
    double alpha_angle = robot.direction >=0 ? robot.direction : M_PI - robot.direction;
    double theta_angle = abs(beta_angle - alpha_angle);   
    return theta_angle; //航向角
}
bool readUntilOK() {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        std::stringstream ss(line);
        int frame, money, K;
        // 读取第一行
        ss >> frame >> money;
        // 读取第二行
        int workbench_count;
        scanf("%d", &workbench_count);

        // 读入所有工作台
        //vector<Workbench> workbenches(workbench_count);
        string line;
        getline(cin, line);// 读入一行空行
        for (int i = 0; i < workbench_count; i++) {
            getline(cin, line);
            workbenches.push_back(parse_workbench(line,i));
        }

        // 读入所有机器人
        //vector<Robot> robots(4);
        for (int i = 0; i < 4; i++) {
            getline(cin, line);
            robots.push_back(parse_robot(line,i));
        }

        // 读入OK结束标志
           // 读入OK结束标志
        getline(cin, line);
        return line == "OK";
        //do something
    }



    return false;
}
// 查找距离机器人最近的工作台
Workbench findNearestWorkbench(Robot robot, vector<Workbench> workbenches) {
    double minDist = INFINITY;
    Workbench nearestWorkbench;
    for (auto workbench : workbenches) {
        double dist = get_distance(robot, workbench);
        if (dist < minDist) {
            minDist = dist;
            nearestWorkbench = workbench;
        }
    }
    return nearestWorkbench;
}

// 机器人前往目标工作台
void goToWorkbench(Robot& robot, Workbench targetWorkbench,int lineSpeed,double angleSpeed) {
    // TODO: 根据具体情况实现机器人前往目标工作台的代码
    //朝向与目标方向夹角小于0.1时，加速向目标移动
    int robotId = robot.id;
    double angleRo = get_direction(robot,targetWorkbench);
    if(angleRo <= 0.1) {
        angleSpeed = 0;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("rotate %d %f\n", robotId, angleSpeed);
    }
    else{
        lineSpeed = 1;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("rotate %d %f\n", robotId, angleSpeed);
    }
}

// 机器人从目标工作台购买物品
void buyFromWorkbench(Robot& robot, Workbench& workbench,int lineSpeed,double angleSpeed) {
    // TODO: 根据具体情况实现机器人购买物品的代码
    int robotId = robot.id;
    double angleRo = get_direction(robot, workbench);
    //到达目标工作台附近停下来，购买材料
    if (robot.workbench_id == workbench.id && robot.carry_item_type == 0) {
        lineSpeed = 0;
        angleSpeed = 0;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("buy %d\n", robotId);
    }
}

// 机器人向目标工作台出售物品
void sellToWorkbench(Robot& robot, Workbench& workbench,int lineSpeed,double angleSpeed) {
    // TODO: 根据具体情况实现机器人出售物品的代码
    int robotId = robot.id;
    double angleRo = get_direction(robot, workbench);
    if(robot.workbench_id == workbench.id) {
            lineSpeed = 0;
            angleSpeed = 0;
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
            printf("sell %d\n", robotId);
    }else if(robot.workbench_id != workbench.id) {        //调整机器人朝向目标工作台，并向其移动
        if(angleRo <= 0.1) {
            angleSpeed = 0;
                //在靠近的时候减速
            if(get_distance(robot, workbench) < 1.5) {
                lineSpeed = 1;
            }
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
        }
        else{
            lineSpeed = 1;
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
            } 
        }
}

int main() {
    readUntilOK();
    puts("OK");
    fflush(stdout);
    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        readUntilOK();
        printf("%d\n", frameID);
        int lineSpeed = 3;
        double angleSpeed = 1.5;
        if(!workbenches.empty()){
            Workbench workb[workbenches.size()];
            Workbench w1, w2, w3, w4, w5, w6, w7, w8;
            Robot b1, b2, b3, b4;
            int type[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            b1 = robots[0];
            b2 = robots[1];
            b3 = robots[2];
            b4 = robots[3];

            // Set up flags for tracking workbench selection
            vector<bool> flags(8, false);

            // Select workbenches for each step
           
            //判断机器人是否携带物品
            // if(b1.carry_item_type == 0){
            //     //前去寻找最近的一处1，2，3类型的工作台购买物品
            //     vector<Workbench> w_123 = {workb[0],workb[1],workb[2]};
            //     Workbench target_w = findNearestWorkbench(r1,w_123);
            //     goToWorkbench(r1, target_w,lineSpeed,angleSpeed);
            //     buyFromWorkbench(r1, target_w,lineSpeed,angleSpeed);
            // }else{
            //     //判断携带物品对应的类型
            //     int carry_type = r1.carry_item_type;
            //     //出售
            //     if(carry_type == 1 && action[4]){
            //         goToWorkbench(r1,workb[4],lineSpeed,angleSpeed);
            //         sellToWorkbench(r1,workb[4],lineSpeed,angleSpeed);
            //     }
            //     if(carry_type == 2 && action[5]){
            //         goToWorkbench(r1,workb[5],lineSpeed,angleSpeed);
            //         sellToWorkbench(r1,workb[5],lineSpeed,angleSpeed);
            //     }
            //     if(carry_type == 3 && action[6]){
            //         goToWorkbench(r1,workb[6],lineSpeed,angleSpeed);
            //         sellToWorkbench(r1,workb[6],lineSpeed,angleSpeed);
            //     }
            // }
        }
        printf("OK\n",frameID);
        fflush(stdout);
    }
    return 0;
}



