#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
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
//向量点积
double dot_product(const Vector A, const Vector B) {
    double dot_product = A.x * B.x + A.y * B.y;
    return dot_product;
}
//向量的模
double vector_magnitude(const Vector A) {
    double sum_of_squares = 0;
    sum_of_squares += A.x * A.x;
    return sqrt(sum_of_squares);
}
//两点之间的距离
double get_distance(const Robot robot, const Workbench workbench) {
    double distance = (robot.x - workbench.x) * (robot.x - workbench.x) + (robot.y - workbench.y) * (robot.y - workbench.y);
    return distance;
}
//获取航向角
double get_direction(const Robot robot, const Workbench workbench) {
    Vector A;             //机器人与目标工作台之间的向量
    Vector robot_unit_vec;//机器人所在位置的x轴正方向的单位向量

    robot_unit_vec = get_unit_vector(robot.x, robot.y);
    A.x = robot.x - workbench.x;
    A.y = robot.y - workbench.y;
    double beta_angle = acos(dot_product(robot_unit_vec, A) / (vector_magnitude(A) * vector_magnitude(robot_unit_vec)));
    double alpha_angle = robot.direction >= 0 ? robot.direction : M_PI - robot.direction;
    double theta_angle = abs(beta_angle - alpha_angle);
    return theta_angle;//航向角
}
//读取或更新工作台和机器人相关参数
bool readUntilOK() {
    int workbenches_count;//工作台数量
    scanf("%d", &workbenches_count);

    //初始化工作台和机器人
    if (workbenches.size() == 0) {
        for (int i = 0; i < workbenches_count; i++) {
            Workbench workb;
            workb.id = i;
            scanf("%d %lf %lf %d %d %d", &workb.type, &workb.x, &workb.y, &workb.remain_time, &workb.material_status, &workb.product_status);
            workbenches.push_back(workb);
        }
        for (int i = 0; i < 4; i++) {
            Robot r;
            r.id = i;
            scanf("%d %d %lf %lf %lf %lf %lf %lf %lf %lf", &r.workbench_id, &r.carry_item_type, &r.time_value_factor, &r.collision_value_factor, &r.angular_velocity, &r.velocity_x, &r.velocity_y, &r.direction, &r.x, &r.y);

            robots.push_back(r);
        }
    }
    //更新工作台和机器人相关数据
    else {

        for (int i = 0; i < workbenches_count; i++) {
            int type, remain_time, material_status;
            int product_status;
            double x, y;
            scanf("%d %lf %lf %d %d %d", &type, &x, &y, &remain_time, &material_status, &product_status);
            workbenches[i].x = x;
            workbenches[i].y = y;
            workbenches[i].remain_time = remain_time;
            workbenches[i].material_status = material_status;
            workbenches[i].product_status = product_status;
        }
        for (int i = 0; i < 4; i++) {
            int workbench_id, carry_item_type;
            double time_value_factor, collision_value_factor, velocity_x, velocity_y, direction, angular_velocity, x, y;
            scanf("%d %d %lf %lf %lf %lf %lf %lf %lf %lf", &workbench_id, &carry_item_type, &time_value_factor, &collision_value_factor, &angular_velocity, &velocity_x, &velocity_y, &direction, &x, &y);
            robots[i].carry_item_type = carry_item_type;
            robots[i].collision_value_factor = collision_value_factor;
            robots[i].time_value_factor = time_value_factor;
            robots[i].angular_velocity = angular_velocity;
            robots[i].velocity_x = velocity_x;
            robots[i].velocity_y = velocity_y;
            robots[i].direction = direction;
            robots[i].x = x;
            robots[i].y = y;
            robots[i].workbench_id = workbench_id;
        }
    }
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something
    }
    return false;
}
// 查找距离机器人最近的工作台
Workbench findNearestWorkbench(Robot robot, vector<Workbench> workbenches) {
    double minDist = INFINITY;
    Workbench nearestWorkbench;
    for (auto workbench: workbenches) {
        double dist = get_distance(robot, workbench);
        if (dist < minDist) {
            minDist = dist;
            nearestWorkbench = workbench;
        }
    }
    return nearestWorkbench;
}

// 机器人前往目标工作台
void goToWorkbench(Robot &robot, Workbench targetWorkbench, int &lineSpeed, double &angleSpeed) {
    // TODO: 根据具体情况实现机器人前往目标工作台的代码
    //朝向与目标方向夹角小于0.1时，加速向目标移动
    int robotId = robot.id;
    double angleRo = get_direction(robot, targetWorkbench);
    if (angleRo <= 0.1) {
        angleSpeed = 0;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("rotate %d %f\n", robotId, angleSpeed);
        fflush(stdout);
    } else {
        lineSpeed = 1;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("rotate %d %f\n", robotId, angleSpeed);
        fflush(stdout);
    }
}

// 机器人从目标工作台购买物品
void buyFromWorkbench(Robot &robot, Workbench &workbench, int &lineSpeed, double &angleSpeed) {
    int robotId = robot.id;
    double angleRo = get_direction(robot, workbench);
    //到达目标工作台附近停下来，购买材料
    if (robot.workbench_id == workbench.id && robot.carry_item_type == 0) {
        lineSpeed = 0;
        angleSpeed = 0;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("buy %d\n", robotId);
        fflush(stdout);
    }
}

// 机器人向目标工作台出售物品
void sellToWorkbench(Robot &robot, Workbench &workbench, int &lineSpeed, double &angleSpeed) {
    // TODO: 根据具体情况实现机器人出售物品的代码
    int robotId = robot.id;
    double angleRo = get_direction(robot, workbench);
    if (robot.workbench_id == workbench.id) {
        lineSpeed = 0;
        angleSpeed = 0;
        printf("forward %d %d\n", robotId, lineSpeed);
        printf("rotate %d %f\n", robotId, angleSpeed);
        printf("sell %d\n", robotId);
        fflush(stdout);
    } else if (robot.workbench_id != workbench.id) {//调整机器人朝向目标工作台，并向其移动
        if (angleRo <= 0.1) {
            angleSpeed = 0;
            //在靠近的时候减速
            if (get_distance(robot, workbench) < 1.5) {
                lineSpeed = 1;
            }
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
            fflush(stdout);
        } else {
            lineSpeed = 1;
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
            fflush(stdout);
        }
    }
}
Workbench chooseWorkbench(Robot &robot, vector<Workbench> &workbenches) {
    for (int i = 0; i < workbenches.size(); i++) {
        if (workbenches[i].type == 1 || workbenches[i].type == 2 || workbenches[i].type == 3) {
            return workbenches[i];
        }
    }
}

int main() {
    int frameID;
    int money;
    scanf("%d %d", &frameID, &money);
    readUntilOK();
    puts("OK");
    fflush(stdout);

    while (scanf("%d", &frameID) != EOF) {
        scanf("%d", &money);
        readUntilOK();
        printf("%d\n", frameID);
        int lineSpeed = 3;
        double angleSpeed = 1.5;
        if (!workbenches.empty()) {
            Workbench wb[4];
            Robot r1, r2, r3, r4;
            r1 = robots[0];
            r2 = robots[1];
            r3 = robots[2];
            r4 = robots[3];
            wb[0] = chooseWorkbench(r1, workbenches);
            goToWorkbench(r1, wb[0], lineSpeed, angleSpeed);
            if (r1.carry_item_type != 0) {
                buyFromWorkbench(r1, wb[0], lineSpeed, angleSpeed);
            } else {
                sellToWorkbench(r1, wb[0], lineSpeed, angleSpeed);
            }
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    return 0;
}
