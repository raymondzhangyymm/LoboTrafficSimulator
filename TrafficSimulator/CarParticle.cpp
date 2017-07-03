#include "CarParticle.h"

//=========================================================================================
CarParticle::CarParticle()
{
    cell_p = NULL;

    carmesh = NULL;

    isBad = false;
    isColliding = false;
    isVisible = true;
}

CarParticle::CarParticle(BezierSequence *road_curve_p_, double road_offset_)
{
    road_curve_p = road_curve_p_;
    road_offset = road_offset_;

    tail_length = 0;
    head_length = 0;

    carmesh = NULL;
    trajectory = NULL;

    isBad = false;
    isColliding = false;
    isVisible = true;
}

CarParticle::CarParticle(Cell *cell_p_)
{
    cell_p = cell_p_;

    carmesh = NULL;

    isBad = false;
    isColliding = false;
    isVisible = true;
}

//=========================================================================================
void CarParticle::initByXPosition(double x_pos_)
{
    x_position = x_pos_;

    x_position_pre = x_position;

    density = 0.0;
    x_velocity = 0.0;

    if (carmesh)
    {
        length = carmesh->length;
        width = carmesh->width;
        height = carmesh->height;
    }

    orientation = road_curve_p->getTangentByXPosition(x_position);
    normal = road_curve_p->getNormalByXPosition(x_position);
    position = road_curve_p->getPointByXPosition(x_position) + normal * road_offset;

    lanechanging_status = 0;
    lanechanging_percentage = 0;
    needLaneChanging = false;

    spring_acceleration.clear();
    spring_velocity.clear();
    spring_position.clear();

    spring_displacement.clear();
    spring_previous_displacement = 0.0;
    spring_angular_frequency = 1000.0;
    spring_damping_ratio = 1.0;
}

void CarParticle::initByPosition(zVec2 pos_)
{
    position = pos_;

    density = 0.0;
    velocity.clear();

    if (carmesh)
    {
        length = carmesh->length;
        width = carmesh->width;
        height = carmesh->height;
    }

    orientation = cell_p->center_tangent;
    normal = cell_p->center_normal;

    lanechanging_status = 0;
    lanechanging_percentage = 0;
    needLaneChanging = false;

    spring_acceleration.clear();
    spring_velocity.clear();
    spring_position.clear();

    spring_displacement.clear();
    spring_previous_displacement = 0.0;
    spring_angular_frequency = 1000.0;
    spring_damping_ratio = 1.0;
}

void CarParticle::getBbox(zVec2 bbox[])
{
    zVec2 v1 = orientation*length*0.5;
    zVec2 v2 = normal*width*0.5;
    bbox[0] = position + v1 + v2;
    bbox[1] = position - v1 + v2;
    bbox[2] = position - v1 - v2;
    bbox[3] = position + v1 - v2;
}

void CarParticle::step(double delta_t)
{
    position += velocity * delta_t;
}

void CarParticle::stepTry(double delta_t)
{
    road_id_try = road_id;
    cell_id_try = cell_id;
    velocity_try = velocity + spring_acceleration * delta_t;
    position_try = position + velocity_try * delta_t;
}

void CarParticle::stepTry(double delta_t, zVec2 vel)
{
    road_id_try = road_id;
    cell_id_try = cell_id;
    velocity_try = vel;
    position_try = position + velocity_try * delta_t;
}

void CarParticle::stepConfirm()
{
    velocity = velocity_try;
    position = position_try;
    road_id = road_id_try;
    cell_id = cell_id_try;
}

void CarParticle::computeSpringAcceleration(double delta_t)
{
    // the direction
    double curr_displacement = spring_displacement.length();
    zVec2 direction = spring_displacement;
    direction.normalize();
//    spring_acceleration = direction * ((curr_displacement - spring_previous_displacement)
//                          *(-2.0*spring_damping_ratio*spring_angular_frequency/delta_t)
//                          - curr_displacement * spring_angular_frequency);
    spring_acceleration = direction * (- curr_displacement * spring_angular_frequency);

//    spring_acceleration = (spring_displacement - spring_displacement_pre)
//                          *(-2.0*spring_damping_ratio*spring_angular_frequency/delta_t)
//                          - spring_displacement * spring_angular_frequency;
//    spring_velocity += spring_acceleration * delta_t;
//    spring_position += spring_velocity * delta_t;

    spring_previous_displacement = curr_displacement;
}

void CarParticle::clearSpring()
{
    spring_acceleration.clear();
    spring_velocity.clear();
    spring_position.clear();
    spring_displacement.clear();
    spring_previous_displacement = 0.0;
}

void CarParticle::stepX(double delta_t)
{
    x_position += x_velocity * delta_t;
}

void CarParticle::stepX(double delta_t, CarParticle *p_car_pre)
{
    double safe_pos = p_car_pre->x_position - p_car_pre->length*0.5;

    double pos = x_position + x_velocity * delta_t;
    if (pos + length <= safe_pos)
    {
        x_position = pos;
    }
}

bool CarParticle::isSafeToPreviousCar(CarParticle *p_car_pre)
{
    if (!p_car_pre)
    {
        return true;
    }

    double safe_pos = p_car_pre->x_position - p_car_pre->length*0.5;
    if (x_position + length <= safe_pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CarParticle::isSafeToNextCar(CarParticle *p_car_next)
{
    if (!p_car_next)
    {
        return true;
    }

    double safe_pos = p_car_next->x_position + p_car_next->length*0.5;
    if (x_position - length >= safe_pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

CarParticle* CarParticle::duplicate()
{
    CarParticle *p_car = new CarParticle(road_curve_p, road_offset);

    p_car->id = id;
    p_car->road_id = road_id;
    p_car->lane_id = lane_id;
    p_car->cell_id = cell_id;

    p_car->carmesh_id = carmesh_id;
    p_car->carcolor_id = carcolor_id;
    p_car->carmesh = carmesh;
    p_car->carcolor = carcolor;

    p_car->x_position = x_position;
    p_car->x_position_pre = x_position_pre;
    p_car->x_velocity = x_velocity;
    p_car->density = density;

    p_car->length = length;
    p_car->width = width;
    p_car->height = height;

    p_car->orientation = orientation;
    p_car->normal = normal;

    return p_car;
}

void CarParticle::pushTail(zVec2 lp, zVec2 rp)
{
    if (tail_length >= 100)
    {
        tail_left[tail_length] = lp;
        tail_right[tail_length] = rp;
        for (int i=0; i<tail_length; i++)
        {
            tail_left[i] = tail_left[i+1];
            tail_right[i] = tail_right[i+1];
        }
    }
    else
    {
        tail_left[tail_length] = lp;
        tail_right[tail_length] = rp;
        tail_length++;
    }
}

void CarParticle::resetTail()
{
    tail_length = 0;
}

void CarParticle::computeXVelocityByTrajectory()
{
    int middle = searchTrajectory(x_position);

    // The x_position could be either greater or smaller than the middle point.
    if (x_position < trajectory->projected_x[middle])
    {
        if (middle > 0)
        {
            x_velocity = (trajectory->projected_x[middle] - trajectory->projected_x[middle-1])
                    / (trajectory->input_timestamp[middle] - trajectory->input_timestamp[middle-1]);
        }
        else
        {
            x_velocity = 0.0;
        }
    }
    else
    {
        if (middle < trajectory->projected_x.size() - 1)
        {
            x_velocity = (trajectory->projected_x[middle+1] - trajectory->projected_x[middle])
                    / (trajectory->input_timestamp[middle+1] - trajectory->input_timestamp[middle]);
        }
        else
        {
            x_velocity = 0.0;
        }
    }

    return;
}

int CarParticle::searchTrajectory(double x_pos)
{
    int size_array = trajectory->projected_x.size();
    int low = 0, high = size_array - 1, middle;
    while (low <= high)
    {
        middle = (low + high) / 2;
        if (x_pos == trajectory->projected_x[middle])
        {
            return middle;
        }
        else if (x_pos < trajectory->projected_x[middle])
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    return middle;
}

void CarParticle::updatePositionByXPos()
{
    orientation = road_curve_p->getTangentByXPosition(x_position);
    normal = road_curve_p->getNormalByXPosition(x_position);
    position = road_curve_p->getPointByXPosition(x_position) + normal * road_offset;
}

void CarParticle::updateTail()
{
    double tail_xpos = x_position - length*0.5;
    zVec2 nl = road_curve_p->getNormalByXPosition(tail_xpos);
    zVec2 tail_position = road_curve_p->getPointByXPosition(tail_xpos) + nl * road_offset;
    zVec2 delta_v = nl * (DEFAULT_LANE_WIDTH * 0.3);
    pushTail(tail_position + delta_v, tail_position - delta_v);
}

void CarParticle::updateHead()
{
    zVec2 nl, pos, delta_v;

    // update the head
    double xpos = x_position + length*0.5;
    double len_xstep = DEFAULT_LANE_WIDTH * 0.2;
    for (int i=0; i<head_length; ++i)
    {
        nl = road_curve_p->getNormalByXPosition(xpos);
        pos = road_curve_p->getPointByXPosition(xpos) + nl * road_offset;
        delta_v = nl * (DEFAULT_LANE_WIDTH * 0.1);

        head_left[i] = pos + delta_v;
        head_right[i] = pos - delta_v;

        xpos += len_xstep;
    }

    // update visual trajectory points in head
    head_visual_trajpoints.clear();
    xpos = x_position + length*0.5;
    int low = searchTrajectory(xpos);
    if (xpos > trajectory->projected_x[low] && low < trajectory->projected_x.size()-1)
    {
        low++;
    }

    xpos += len_xstep*head_length;
    int high = searchTrajectory(xpos);
    if (xpos < trajectory->projected_x[high] && high > 0)
    {
        high--;
    }

    for (int j=low; j<=high; ++j)
    {
        head_visual_trajpoints.push_back(trajectory->projected_point[j]
                                         + trajectory->projected_normal[j]*road_offset);
    }
}
