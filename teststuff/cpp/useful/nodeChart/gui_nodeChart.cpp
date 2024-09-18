

#include "globals_includes.hpp"


gNC::gNODE* gNC::nodePtr_menu__node_details = nullptr;
gNC::gNODE* gNC::nodePtr_menu__rightClick   = nullptr;
gNC::gNODE* gNC::nodePtr__dragConnectCreate_start = nullptr;

gNC::gLINK* gNC::linkPtr_menu__link_details = nullptr;
gNC::gLINK* gNC::linkPtr_menu__rightClick   = nullptr;

gNC::gLINK  gNC::dragConnectCreate_tempLink;
int         gNC::option_dragConnectCreate_tempLink_snapIn = 0;
int         gNC::dragConnectCreate_startedEnd;


gNC::gNODE::gNODE(
    float par_posX, float par_posY,
    std::string par_label, std::string par_desc, std::string par_bodyText,
    std::vector<gNC::gLINK*> par_ln_in,  std::vector<gNC::gLINK*> par_ln_out,
    std::vector<gNC::gLINK*> par_ln_add, std::vector<gNC::gLINK*> par_ln_share,
    int par_layout,
    float par_width,     float par_height,
    float par_posX_in,   float par_posY_in,
    float par_posX_out,  float par_posY_out,
    float par_posX_add,  float par_posY_add,
    float par_posX_share,float par_posY_share

): label{par_label}, desc{par_desc}, bodyText{par_bodyText}, ln_in{par_ln_in}, ln_out{par_ln_out}, ln_add{par_ln_add}, ln_share{par_ln_share} {
    if(checkExistence<int>(par_layout, std::vector<int>{0,1,2,3})==-1) std::runtime_error("ERROR: gNC::gNODE constructor: par_layout is an invalid value");
    layout = par_layout;
    width   = par_width;
    height  = par_height;
    pos[0]  = par_posX;
    pos[1]  = par_posY;
    pos_in[0]   = par_posX_in;  pos_in[1]   = par_posY_in;
    pos_out[0]  = par_posX_out; pos_out[1]  = par_posY_out;
    pos_add_0[0]    = par_posX_add;     pos_add_0[1]    = par_posY_add;
    pos_share_0[0]  = par_posX_share;   pos_share_0[1]  = par_posY_share;
    // std::cout<<pos_add_0[0]<< " "<<pos_add_0[1]<<" | " <<pos_share_0[0]<< " "<<pos_share_0[1] << " - ";
    if(layout==0 || layout==1) { //define the opposing connection points positions
        pos_add_1[0]    = pos_add_0[0];
        pos_add_1[1]    = height-pos_add_0[1];
        pos_share_1[0]  = pos_share_0[0];
        pos_share_1[1]  = height-pos_share_0[1];
    }
    else if(layout==2 || layout==3) {
        pos_add_1[1]    = pos_add_0[1];
        pos_add_1[0]    = width-pos_add_0[0];
        pos_share_1[1]  = pos_share_0[1];
        pos_share_1[0]  = width-pos_share_0[0];
    }
    // std::cout<<pos_add_1[0]<< " "<<pos_add_1[1]<<" | " <<pos_share_1[0]<< " "<<pos_share_1[1] << std::endl;

}

void gNC::gNODE::setPos(int x, int y) {
    this->pos[0]= x;
    this->pos[1]= y;
}
void gNC::gNODE::setDim(int w, int h) {
    this->width = w;
    this->height= h;
}

/**
 * Get the relative (to node subspace) position of a node's connection point from the connection id
 * 
 * ### Parameters:
 * `connectionID` integer identifier for each of the connection points:
 *  - `0` - `pos_in`
 *  - `1` - `pos_out`
 *  - `2` - `pos_add_0`
 *  - `3` - `pos_share_0`
 *  - `4` - `pos_add_1`
 *  - `5` - `pos_share_1`
 * ### Return:
 *  `ImVec2` of the relative coordinate
 */
ImVec2 gNC::gNODE::getConnectionPos(int connectionID) {

    switch (connectionID) {
    case  0: return pos_in;      break;
    case  1: return pos_out;     break;
    case  2: return pos_add_0;   break;
    case  3: return pos_share_0; break;
    case  4: return pos_add_1;   break;
    case  5: return pos_share_1; break;
    default:
        std::runtime_error("ERROR: gNC::gNODE::getConnectionPos(): connectionID is incorrect");
    }
    return ImVec2();
}

/**
 * 
 * ### Parameters:
 * `typeDef` integer identifier for what type to get.
 *  - `2` - `add`   point positive from center (below center)
 *  - `3` - `share` point positive from center (below center)
 *  - `4` - `add`   point negative from center (above center)
 *  - `5` - `share` point negative from center (above center)
 * ### Return:
 *  integer to type to return that's acceptible by `gNC::gNODE::getConnectionPos(..)`
 */
int gNC::gNODE::getConnectionType(int typeDef) {
    
    switch (typeDef) {
    case 5: return 3; break;
    case 4: return 2; break;
    case 3: return 5; break;
    case 2: return 4; break;
    default:
        std::runtime_error("ERROR: gNC::gNODE::getConnectionType(..): typeDef is incorrect");
    }
    return -1;
}

void gNC::gNODE::draw_connection(
    std::vector<ImDrawList*> draw_win,
    ImVec2 nodePos
) {
    static ImU32 colour_bg      = IM_COL32(100, 100, 100, 255);
    static ImU32 colour_hover   = IM_COL32(100, 100, 100, 160);
    static ImU32 colour_press   = IM_COL32(150, 150, 150, 160);
    static ImU32 colour_border  = IM_COL32(200, 200, 200, 255);

    for(int i=0; i<6; i++) {
        ImVec2 tempPos = getConnectionPos(i);
        tempPos.x += nodePos.x;
        tempPos.y += nodePos.y;
            
        // (
        // i==0?   ImVec2(pos_in[0]     + nodePos.x, pos_in[1]     + nodePos.y) :
        // (i==1?  ImVec2(pos_out[0]    + nodePos.x, pos_out[1]    + nodePos.y) :
        // (i==2?  ImVec2(pos_add_0[0]  + nodePos.x, pos_add_0[1]  + nodePos.y) :
        // (i==3?  ImVec2(pos_share_0[0]+ nodePos.x, pos_share_0[1]+ nodePos.y) :
        // (i==4?  ImVec2(pos_add_1[0]  + nodePos.x, pos_add_1[1]  + nodePos.y) :
        //         ImVec2(pos_share_1[0]+ nodePos.x, pos_share_1[1]+ nodePos.y)))))
        // );

        for(ImDrawList* el: draw_win) {
            el->AddCircleFilled(tempPos, ROI_attach[0]*0.5, colour_bg, 50);
            el->AddCircle(      tempPos, ROI_attach[0]*0.5, colour_border, 50, 3);

            switch (state_connections[i]) {
            case 1: el->AddCircleFilled(tempPos, ROI_attach[0]*0.5, colour_hover, 50); break;
            case 2: el->AddCircleFilled(tempPos, ROI_attach[0]*0.3, colour_press, 50); break;
            default:
                break;
            }

        }
    }

}


gNC::gLINK::gLINK() {}
gNC::gLINK::gLINK(
    int par_type_src, int par_type_dest,
    gNC::gNODE* par_src, gNC::gNODE* par_dest,
    std::string par_label, std::string par_desc
): Pos_src(-1,-1), Pos_dest(-1,-1)/*, Pos_s1(-1,-1), Pos_d1(-1,-1)*/ {
    if(searchVec<int>(std::vector<int>{1, 3, 5}, par_type_src)==-1 && searchVec<int>(std::vector<int>{0, 2, 4}, par_type_dest)==-1) std::runtime_error("ERROR: gNC::gLINK() constructor par_type_{..} is invalid");
    if(par_src==nullptr && par_dest==nullptr) std::runtime_error("ERROR: gNC::gLINK(): constructor: both `src` and `dest` can't be nullptr");

    this->type_src  = par_type_src;
    this->type_dest = par_type_dest;
    this->src  = par_src;
    this->dest = par_dest;
    this->label = par_label;
    this->desc  = par_desc;
    _init = true;
}

/**
 * @brief move the coordinates for the points that make up LINK draw
 * 
 * @param par_pos_src absolute 2d position of _src point
 * @param par_pos_dest absolute 2d position of _dest point
 */
void gNC::gLINK::move_link(
    ImVec2 par_pos_src,
    ImVec2 par_pos_dest
) {
    /**
     * If value is -2, that value is kept.
     * If value is -1, that value allowed to change (applies only to s1 and d1)
     * else: update it
     */

    assert(_init);

    if(par_pos_src.x  != -2) Pos_src.x  = par_pos_src.x;
    if(par_pos_src.y  != -2) Pos_src.y  = par_pos_src.y;
    if(par_pos_dest.x != -2) Pos_dest.x = par_pos_dest.x;
    if(par_pos_dest.y != -2) Pos_dest.y = par_pos_dest.y;


    ImVec2 pos_delta = ImVec2(Pos_dest.x - Pos_src.x, Pos_dest.y - Pos_src.y);
    ImVec2 pos_middle= ImVec2(Pos_src.x + pos_delta.x/2, Pos_src.y + pos_delta.y/2);
    this->Pos_center = pos_middle;


    /// @brief half of the smallest delta
    std::vector<float> tempVec{abs(pos_delta.x), abs(pos_delta.y)};
    float smallestDelta = findVal(tempVec, 1)/2;
    /// @brief `0`- x is smallest; `1`- y is smallest
    int smallestType    = (pos_delta.x < pos_delta.y? 0 : 1);


    /**
     * The direction which the connectionPos is located relative to the center. Only used if both type_src and type_dest are in the same direction
     * ` 1` - positive down
     * `-1` - negative up
    */
    int connDir_dest= -1;
    int connDir_src = -1;
    if(type_dest!= 0 && dest) connDir_dest= PoN(dest->getConnectionPos(type_dest).y - dest->height/2); // / abs(dest->getConnectionPos(type_dest).y - dest->height/2);
    if(type_src != 1 && src) connDir_src = PoN(src->getConnectionPos(type_src).y - src->height/2); /// abs(src->getConnectionPos(type_src).y - src->height/2);


    link_points.clear();
    link_points.push_back(Pos_src);

    /**
     * Assumptions:
     * - connectionPoints for _src and _dest have been correctly defined from the beginning;
     * 
     */

    // Values for same side connectionPos methods

    tempVec = std::vector<float>{min__connect, smallestDelta};
    float offs_bigger   = findVal(tempVec, 0);
    float offs_smaller  = findVal(tempVec, 1);
    tempVec = std::vector<float>{Pos_src.y, Pos_dest.y};
    /**
     * The y value of the link node that is the furthest away from median line between the link nodes,
     * in the direction of the link relative to conenctionPos's
     * 
     * i.e.
     * - is the link facing positive y (down)? find the biggest y value
     * - is the link facing negative y (up)? find the smallest y value
     */
    float y_bigger = findVal(tempVec, (connDir_src==1? 0 : 1));


    if(type_src==1) {
        if(layout==0 || layout==1) {
            link_points.push_back(ImVec2(Pos_src.x + (type_dest==0? pos_delta.x/2 : pos_delta.x) - smallestDelta, Pos_src.y));
            link_points.push_back(ImVec2(Pos_src.x + (type_dest==0? pos_delta.x/2 : pos_delta.x), Pos_src.y));
            if(type_dest==0) {
                link_points.push_back(ImVec2(pos_middle.x, Pos_src.y+PoN(pos_delta.y)*smallestDelta));
                link_points.push_back(pos_middle);
            }
        }
        else {
            link_points.push_back(ImVec2(Pos_src.x, Pos_src.y + (type_dest==0? pos_delta.y/2 : pos_delta.y) - smallestDelta));
            link_points.push_back(ImVec2(Pos_src.x, Pos_src.y + (type_dest==0? pos_delta.y/2 : pos_delta.y)));
            if(type_dest==0) link_points.push_back(pos_middle);
        }
    }
    else /*if(type_src==3) */ {
        if(layout==0 || layout==1) {

            if(type_dest==0) {
                link_points.push_back(ImVec2(Pos_src.x, Pos_src.y + (pos_delta.y - connDir_src*smallestDelta)));
                link_points.push_back(ImVec2(Pos_src.x, Pos_src.y + pos_delta.y));
            }
            else if(type_dest==(type_src==3? 2 : 4)) { // same side

                // if delta_x is bigger: use min__connect: else use smallestDelta

                link_points.push_back(ImVec2(
                    Pos_src.x,
                    y_bigger + offs_bigger*connDir_src - (pos_delta.x>pos_delta.y? offs_bigger : offs_smaller)*connDir_src
                ));
                link_points.push_back(ImVec2(
                    Pos_src.x,
                    y_bigger + offs_bigger*connDir_src
                ));
                link_points.push_back(ImVec2(
                    Pos_src.x + (pos_delta.x>pos_delta.y? offs_bigger : offs_smaller)*PoN(pos_delta.x),
                    link_points.back().y
                ));
                link_points.push_back(ImVec2(
                    pos_middle.x,
                    y_bigger + offs_bigger*connDir_src
                ));
            }
            else { //type_dest==4 if type_src==3; else type_dest==2
                link_points.push_back(ImVec2(
                    Pos_src.x,
                    Pos_src.y + (pos_delta.y/2 - smallestDelta*connDir_src)
                ));
                link_points.push_back(ImVec2(
                    Pos_src.x,
                    Pos_src.y + pos_delta.y/2
                ));
                link_points.push_back(ImVec2(
                    Pos_src.x + PoN(pos_delta.x)*smallestDelta,
                    pos_middle.y
                ));
                link_points.push_back(pos_middle);
            }
        }
        else {

        }
    }
    // else if(type_src==5) {
    //     if(layout==0 || layout==1) {
    //         if(type_dest==0) link_points.push_back(ImVec2(Pos_src.x, Pos_src.y + (pos_delta.y - smallestDelta)));
    //         else if(type_dest==4) {
    //             link_points.push_back(ImVec2(
    //                 Pos_src.x,
    //                 findVal(std::vector<float>{Pos_src.y, Pos_dest.y}, (connDir==-1? 0 : 1)) +
    //                 findVal(std::vector<float>{connDir*min__connect, smallestDelta}, 0) -
    //                 findVal(std::vector<float>{connDir*min__connect, smallestDelta}, 1)
    //             ));
    //         }
    //         else {
    //             link_points.push_back(ImVec2(
    //                 Pos_src.x,
    //                 Pos_src.y + (pos_delta.y/2 - smallestDelta)
    //             ));
    //         }
    //     }
    //     else {
    //     }
    // }


    if(type_dest==0) {
        if(layout==0 || layout==1) {
            if(type_src==1) {
                link_points.push_back(ImVec2(pos_middle.x, Pos_dest.y - PoN(pos_delta.y)*smallestDelta));
                link_points.push_back(ImVec2(pos_middle.x, Pos_dest.y));
            }
            link_points.push_back(ImVec2(link_points.back().x + smallestDelta, Pos_dest.y));
        }
        else {
            // if(type_src==1) link_points.push_back(ImVec2(pos_middle.x + smallestDelta, pos_middle.y));
            // link_points.push_back(ImVec2(Pos_dest.x, link_points.back().y + smallestDelta));
        }
    }
    else {
        if(layout==0 || layout==1) {
            if(type_src==1) { // 1 - 2/4
                link_points.push_back(ImVec2(Pos_dest.x, link_points.back().y - (type_src-type_dest==1 && type_src!=1? offs_bigger : smallestDelta)*connDir_dest));
            }
            else if(type_src-type_dest==1 && type_src!=1) { //same side
                link_points.push_back(ImVec2(
                    Pos_dest.x - PoN(pos_delta.x)*(pos_delta.x>pos_delta.y? offs_bigger : offs_smaller),
                    link_points.back().y
                ));
                link_points.push_back(ImVec2(Pos_dest.x, link_points.back().y));
                link_points.push_back(ImVec2(
                    Pos_dest.x, link_points.back().y - (connDir_dest)*(pos_delta.x>pos_delta.y? offs_bigger : offs_smaller)
                ));
            }
            else { //opposite side
                link_points.push_back(ImVec2(
                    Pos_dest.x - PoN(pos_delta.x)*smallestDelta,
                    pos_middle.y
                ));
                link_points.push_back(ImVec2(Pos_dest.x, link_points.back().y));
                link_points.push_back(ImVec2(
                    Pos_dest.x, link_points.back().y - (connDir_dest)*(smallestDelta)
                ));
            }
        }
        else {

        }
    }


    link_points.push_back(Pos_dest);

}
void gNC::gLINK::draw_link(
    std::vector<ImDrawList*> draw_win,
    ImVec2 screen_offset
) {
    /**
     * Need to find a way for a link between two nodes to be highlighted when
     *  - this link is clicked/focused
     *  - either node of this link is clicked/focused
     * 
     * when drawing the line on the node, only draw to the first intermediary point.
     */
    assert(_init);
    static ImU32 colour_bg      = IM_COL32(250, 241, 58, 204);
    static ImU32 colour_bg_hover= IM_COL32(250, 241, 150,204);

    static ImU32 colour_border  = IM_COL32(100, 100, 100, 255); 


    ImU32 linkColour = (draw__state? colour_bg_hover : colour_bg);


    static bool draw__lines = false;
    static bool draw__points= false;

    /**
     * @brief add the relative drag screen offset
     * @param toAdd `ImVec2` container for the 2d coordinate to add the screen_offset to
     * @return `ImVec2` of the 2d coordinate with the screen_offset added
     */
    auto addOffs = [screen_offset](ImVec2 toAdd) {
        return ImVec2(toAdd.x + screen_offset.x, toAdd.y + screen_offset.y);
    };


    /// number of segments to create for the quadratic bezier curve
    int bezierSegs = 10;

    /// erase points with the same position
    for(int i=0; i<link_points.size()-1; i++) {
        if(link_points[i].x == link_points[i+1].x && link_points[i].y == link_points[i+1].y) {
            auto itr = link_points.begin();
            advance(itr, i);
            link_points.erase(itr);
            i--;
        }
    }

    /**
     * @brief Dedicated function for solving direction identifier
     * @param p0 `ImVec2` value of point 0
     * @param p1 `Imvec2` value of point 1
     * @return dedicated integer for the directions:
     *  - `0` - positive x
     *  - `1` - positive y
     *  - `2` - negative x
     *  - `3` - negative y
     */
    static auto _define_prevAxis = [](ImVec2 p0, ImVec2 p1) {
        assert(!(p0.x==p1.x && p0.y==p1.y));
        ImVec2 delta(p1.x-p0.x, p1.y-p0.y);
        if(delta.x>0) return 0;
        if(delta.y>0) return 1;
        if(delta.x<0) return 2;
        if(delta.y<0) return 3;
        return -1;
    };

    /**
     * Identifier for the direction at which the previous two points went in.
     *  used to distinguish when a point is not in the same direction as the previous ones.
     *  - `0` - positive x
     *  - `1` - positive y
     *  - `2` - negative x
     *  - `3` - negative y
     */
    static int prevAxis = 0;

    link_points_raw.clear();
    link_points_coeffs.clear();
    // link_points_raw.push_back(link_points[0]);

    /// Draw the points onto the window(s)
    for(int i=0; i<link_points.size()-2; i++) {

        prevAxis = _define_prevAxis(link_points[i], link_points[i+1]);
        if(_define_prevAxis(link_points[i+1], link_points[i+2]) != prevAxis) {
            std::vector<pos2d> curve = quadratic_bezier(to_pos2d(link_points[i]), to_pos2d(link_points[i+2]), to_pos2d(link_points[i+1]), bezierSegs, &link_points_coeffs, "1");
            // std::cout << "----------"<<std::endl;
            // std::cout << curve[0].getStr() << std::endl;
            link_points_raw.push_back(to_ImVec2(curve[0]));
            for(int ii=1; ii<curve.size(); ii++) {
                // draw_win[0]->AddLine(addOffs(to_ImVec2(curve[ii-1])), addOffs(to_ImVec2(curve[ii])), IM_COL32(255, 10, 10, 200), link_lineWidth);
                link_points_raw.push_back(to_ImVec2(curve[ii]));
                // std::cout << curve[ii].getStr() << std::endl;
            }
            // std::cout << "----------   ----------"<<std::endl;

            // draw_win[0]->AddBezierQuadratic(addOffs(link_points[i]), addOffs(link_points[i+1]), addOffs(link_points[i+2]), colour_border, link_lineWidth, bezierSegs);
            // draw_win[0]->AddBezierQuadratic(addOffs(link_points[i]), addOffs(link_points[i+1]), addOffs(link_points[i+2]), colour_bg, link_lineWidth*0.7, bezierSegs);
            
            i+=1;
        }
        else {
            // draw_win[0]->AddLine(addOffs(link_points[i]), addOffs(link_points[i+1]), colour_border, link_lineWidth);
            // draw_win[0]->AddLine(addOffs(link_points[i]), addOffs(link_points[i+1]), colour_bg, link_lineWidth*0.7);
            link_points_raw.push_back(link_points[i]);
            link_points_coeffs.push_back(getCoef_linear(to_pos2d(link_points[i]), to_pos2d(link_points[i+1])));
        }

    }
    if(
        _define_prevAxis(link_points[link_points.size()-3], link_points[link_points.size()-2]) ==
        _define_prevAxis(link_points[link_points.size()-2], link_points[link_points.size()-1])
    ) {
        size_t lineIDX[2] = {link_points.size()-2, link_points.size()-1};
        link_points_raw.push_back(link_points[lineIDX[1]]);
        link_points_coeffs.push_back(getCoef_linear(to_pos2d(link_points[lineIDX[0]]), to_pos2d(link_points[lineIDX[1]])));

        // draw_win[0]->AddLine(addOffs(link_points[lineIDX[0]]), addOffs(link_points[lineIDX[1]]), colour_border, link_lineWidth);
        // draw_win[0]->AddLine(addOffs(link_points[lineIDX[0]]), addOffs(link_points[lineIDX[1]]), linkColour, link_lineWidth*0.7);
    }
    link_points_raw__updated = false;

    // draw_win[0]->AddCircle(addOffs(link_points_raw[0]), 10, IM_COL32(255, 20, 20, 250), 10, 2);
    for(int i=1; i<link_points_raw.size(); i++) {
        draw_win[0]->AddLine(addOffs(link_points_raw[i-1]), addOffs(link_points_raw[i]), colour_border, link_lineWidth);
        draw_win[0]->AddLine(addOffs(link_points_raw[i-1]), addOffs(link_points_raw[i]), linkColour, link_lineWidth*0.7);
        // draw_win[0]->AddCircle(addOffs(link_points_raw[i]), 10, IM_COL32(255, 20, 20, 250), 10, 2);
        // draw_win[0]->AddLine(addOffs(link_points_raw[i-1]), addOffs(link_points_raw[i]), IM_COL32(255, 20, 20, 250), 2);
    }

    if(draw__lines || draw__points) {
        for(int i=0; i<link_points.size()-1; i++) {
            for(ImDrawList* el: draw_win) {
                if(draw__points) {
                    el->AddCircle(addOffs(link_points[i]), 10, IM_COL32(255, 10, 10, 255), 10, 2);
                    el->AddCircle(addOffs(link_points[i]), 3, linkColour, 10, 3);
                }
                if(draw__lines) el->AddLine(addOffs(link_points[i]), addOffs(link_points[i+1]), linkColour);
            }
        }
        for(ImDrawList* el: draw_win) {
            if(draw__points) {
                el->AddCircle(addOffs(link_points[link_points.size()-1]), 10, IM_COL32(255, 10, 10, 255), 10, 2);
                el->AddCircle(addOffs(link_points[link_points.size()-1]), 3, linkColour, 10, 3);
            }
        }
    }

}
bool gNC::gLINK::region(
    ImVec2 cursor,
    ImVec2 _offset
) {
    static bool draw__pointRect = false;

    bool is_in_region = false;
    if(link_points_raw.size()==0) return false;

    if(!link_points_raw__updated) {
        link_lims = {
            std::vector<float>{idx_findVal<ImVec2, float>(link_points_raw, 1, 0), idx_findVal<ImVec2, float>(link_points_raw, 0, 0)},
            std::vector<float>{idx_findVal<ImVec2, float>(link_points_raw, 1, 1), idx_findVal<ImVec2, float>(link_points_raw, 0, 1)}
        };
        link_points_raw__updated = true;
    }
    
    /// check if cursor is outside the outer bounding box: if it is outside, return false and exit the function
    if(!(
        cursor.x-_offset.x > link_lims[0][0]-link_gui__lineWidth && cursor.x-_offset.x < link_lims[0][1]+link_gui__lineWidth &&
        cursor.y-_offset.y > link_lims[1][0]-link_gui__lineWidth && cursor.y-_offset.y < link_lims[1][1]+link_gui__lineWidth
    )) return false;

    assert(link_points_coeffs.size()>0 && link_points_coeffs[0].size()==2);

    for(size_t i=0; i<link_points_raw.size()-1; i++) {
        /**
         * [0] x
         * [1] y
         */
        int nextDir[2] = {
            (link_points_raw[i+1].x > link_points_raw[i].x? 1 : -1),
            (link_points_raw[i+1].y > link_points_raw[i].y? 1 : -1)
        };

        std::vector<ImVec2> _boxs_lim{
            ImVec2(link_points_raw[i].x   - nextDir[0]*link_gui__lineWidth + _offset.x, link_points_raw[i].y   - nextDir[1]*link_gui__lineWidth + _offset.y),
            ImVec2(link_points_raw[i+1].x + nextDir[0]*link_gui__lineWidth + _offset.x, link_points_raw[i+1].y + nextDir[1]*link_gui__lineWidth + _offset.y)
        };

        if(_boxs_lim[0].x > _boxs_lim[1].x) {
            float temp = _boxs_lim[0].x;
            _boxs_lim[0].x = _boxs_lim[1].x;
            _boxs_lim[1].x = temp;
        }
        if(_boxs_lim[0].y > _boxs_lim[1].y) {
            float temp = _boxs_lim[0].y;
            _boxs_lim[0].y = _boxs_lim[1].y;
            _boxs_lim[1].y = temp;
        }

        if(inRegion(cursor, _boxs_lim[0], _boxs_lim[1])) {
            
            if(draw__pointRect) project_draw_list->AddRect(_boxs_lim[0], _boxs_lim[1], IM_COL32(30, 30, 250, 240));
            
            return true;
        }

        if(draw__pointRect) project_draw_list->AddRect(_boxs_lim[0], _boxs_lim[1], IM_COL32(30, 250, 30, 240));

    }

    return is_in_region;
}

const gNC::gNODE gNC::guiNodeChart::default_gNODE = {
    0, 0,
    "", "", ""
};


template<typename storedType>
int gNC::guiNodeChart::_find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found ++;
        if(&*itr==ptr_toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto gNC::guiNodeChart::_find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(&(*itr)==ptr_toFind) {
            return itr;
        }
    }
    return toCheck.end();
}

template<typename storedType>
int gNC::guiNodeChart::_vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(*itr==toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto gNC::guiNodeChart::_vecfind_ptr_itr( std::vector<storedType>& toCheck, storedType toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(*itr==toFind) {
            return itr;
        }
    }
    return toCheck.end();
}


/**
 * @brief 
 * 
 * @param toCheck 
 * @param srcPos absolute 2d position of src connecting pos in reference with node's outer reference frame
 * @param destPos absolute 2d position of dest connecting pos in reference with node's outer reference frame
 */
void gNC::guiNodeChart::update_connect(
    gNC::gLINK* toCheck,
    ImVec2 srcPos,
    ImVec2 destPos
) {
    std::vector<gNC::gNODE*> node = {
        toCheck->src, toCheck->dest
    };

    /**
     * Boolean array to check whether either node of the link `toCheck` is a "loose" end, i.e. a nullptr
     * `[0]` - src
     * `[1]` - dest
     */
    std::vector<bool> isNULL{
        (!node[0]? true : false),
        (!node[1]? true : false)
    };

    if(srcPos[0]==-1) srcPos[0] = toCheck->Pos_src.x;
    if(srcPos[1]==-1) srcPos[1] = toCheck->Pos_src.y;
    if(destPos[0]==-1) destPos[0] = toCheck->Pos_dest.x;
    if(destPos[1]==-1) destPos[1] = toCheck->Pos_dest.y;

    //error
    ImVec2 pos_delta_node(
        (!isNULL[1]? node[1]->pos[0] : destPos[0]) - (!isNULL[0]? node[0]->pos[0] : srcPos[0]),
        (!isNULL[1]? node[1]->pos[1] : destPos[1]) - (!isNULL[0]? node[0]->pos[1] : srcPos[1])
    );


    if(!isNULL[1] && ((toCheck->type_src==3 && toCheck->type_dest==4) || (toCheck->type_src==5 && toCheck->type_dest==2))) { // if connPoints are on opposite sides
        if(
            (abs(node[0]->pos.y+node[0]->height - destPos.y) < toCheck->min__node) ||
            (abs(node[1]->pos.y+node[1]->height - srcPos.y)  < toCheck->min__node)
        ) {
            toCheck->type_dest = (toCheck->type_src==3? 2 : 4); //convert them to same side

        }
    }
    if((toCheck->type_src-toCheck->type_dest==1) && toCheck->type_src!=1) { // if connPoints are on the same side
        if(
            ((abs(pos_delta_node.y) > (toCheck->min__node)*2))
        ) { //if connPoints are on the same side and have not reached minimal distance: convert to opposide side
            
            // toCheck->type_dest = (toCheck->type_src==3? 4 : 2);
            if(
                (isNULL[0]? srcPos[1]  : node[0]->pos.y) < 
                (isNULL[1]? destPos[1] : node[1]->pos.y)
            ) {
                if(!isNULL[0]) toCheck->type_src  = node[0]->getConnectionType(3);
                if(!isNULL[1]) toCheck->type_dest = node[1]->getConnectionType(4);
            }
            else {
                if(!isNULL[0]) toCheck->type_src  = node[0]->getConnectionType(5);
                if(!isNULL[1]) toCheck->type_dest = node[1]->getConnectionType(2);
            }
            
        }
    }
    if(!isNULL[0] && ((toCheck->type_src ==3 || toCheck->type_src ==5) && toCheck->type_dest==0)) { // one side is share, other is in
        if(toCheck->src->pos[1] + toCheck->src->height/2 < destPos[1]) {
            toCheck->type_src = toCheck->src->getConnectionType(3);
        }
        else if(toCheck->src->pos[1]+toCheck->src->height/2 > destPos[1]) {
            toCheck->type_src = toCheck->src->getConnectionType(5);
        }  
    }
    if(!isNULL[1] && ((toCheck->type_dest==2 || toCheck->type_dest==4) && toCheck->type_src ==1)) { // one side is add, other is out
        if(toCheck->dest->pos[1]+toCheck->dest->height/2 < srcPos[1]) {
            toCheck->type_dest= toCheck->dest->getConnectionType(2);
        }
        else if(toCheck->dest->pos[1]+toCheck->dest->height/2 > srcPos[1]) {
            toCheck->type_dest= toCheck->dest->getConnectionType(4);
        }
    }

}


gNC::guiNodeChart::guiNodeChart(/* args */) {

}

/**
 * @brief move the background screen position and the objects on it.
 * 
 * @param x new x coordinate value
 * @param y new y coordinate value
 * @param moveMode
 *  - `0` absolute
 *  - `1` relative
 * @return int 
 */
int gNC::guiNodeChart::setScreen_pos(int x, int y, int moveMode) {
    assert(moveMode>0 && moveMode<2);

    switch (moveMode) {
    case 0:
        this->screen_pos_delta[0] = x-this->screen_pos[0];
        this->screen_pos_delta[1] = y-this->screen_pos[1];
        this->screen_pos[0] = x;
        this->screen_pos[1] = y;
        break;
    case 1:
        this->screen_pos_delta[0] = x;
        this->screen_pos_delta[1] = y;
        this->screen_pos[0]+=x;
        this->screen_pos[1]+=y;
        break;
    default:
        break;
    }

    return 0;
}
int gNC::guiNodeChart::setScreen_dim(int w, int h) {
    this->screen_dim[0] = w;
    this->screen_dim[1] = h;
    return 0;
}

size_t gNC::guiNodeChart::size(int whatList) {
    if(whatList==0) return this->_nodes.size();
    else if(whatList==1) return this->_links.size();
    std::runtime_error("ERROR: "+this->_info_name+"size(int): input arguemnt for `whatList` does not match available options");
    return 0;
}
gNC::gNODE* gNC::guiNodeChart::lastAdded_NODE() {
    return this->_lastAddedNode;
}
gNC::gLINK* gNC::guiNodeChart::lastAdded_LINK() {
    return this->_lastAddedLink;
}

gNC::gNODE& gNC::guiNodeChart::operator[](size_t i) const {
    if(i>=this->_nodes.size()) std::runtime_error("ERROR: "+_info_name+"operator[](size_t) index is out of range");

    std::list<gNC::gNODE>::const_iterator itr = _nodes.begin();
    advance(itr, i);
    return const_cast<gNC::gNODE&>(*itr);
}


std::list<gNC::gNODE> gNC::guiNodeChart::NODES() {
    return this->_nodes;
}
std::list<gNC::gLINK> gNC::guiNodeChart::LINKS() {
    return this->_links;
}

gNC::gNODE* gNC::guiNodeChart::NODE_create(
    float pos_x,
    float pos_y,
    std::string label,
    std::string desc,
    std::string bodyText,
    float width,
    float height
) {
    this->_nodes.push_back(gNC::gNODE(
        pos_x, pos_y, label, desc, bodyText
    ));

    this->_lastAddedNode = &(this->_nodes.back());
    this->_lastAddedNode->addr = ptrToStr<gNC::gNODE*>(this->_lastAddedNode);

    return this->_lastAddedNode;
}
int gNC::guiNodeChart::NODE_delete(size_t NODE_idx, bool leaveFloating) {
    if(NODE_idx >= this->_nodes.size()) std::runtime_error(
        "ERROR:: "+this->_info_name+"NODE_delete(size_t, bool):"+" arg for `NODE_idx` is out of range"
    );

    gNC::gNODE* erasePtr;
    std::list<gNC::gNODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_idx);
    return this->NODE_delete(&*itr, leaveFloating);
}
int gNC::guiNodeChart::NODE_delete(gNC::gNODE* NODE_toDelete, bool leaveFloating) {
    std::list<gNC::gNODE>::const_iterator eraseItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toDelete);
    if(eraseItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_delete(gNC::gNODE*, bool):"+
        " arg for node address does not exist in stored nodes."
    );


    if(!leaveFloating) {
        for(size_t i=0; i<4; i++) {
            const std::vector<gNC::gLINK*> linkVec = (
                i==0? NODE_toDelete->ln_in : 
                (i==1? NODE_toDelete->ln_out : 
                (i==2? NODE_toDelete->ln_add : 
                NODE_toDelete->ln_share))
            );

            std::vector<std::list<gNC::gLINK>::const_iterator> itrToErase;

            for(gNC::gLINK* plink: linkVec) {
                if(NODE_toDelete==plink->src) {
                    std::vector<gNC::gLINK*>& plink_type = (plink->type_dest==0? plink->dest->ln_in : plink->dest->ln_add); //corresponding opposite vector
                    for(gNC::gLINK* pplink: plink_type) {
                        if(pplink==plink) {
                            plink_type.erase(this->_vecfind_ptr_itr<gNC::gLINK*>(plink_type, plink));
                        }
                    }
                }
                else if(NODE_toDelete==plink->dest) {
                    std::vector<gNC::gLINK*>& plink_type = (plink->type_src==1? plink->src->ln_out : plink->src->ln_share);
                    for(gNC::gLINK* pplink: plink_type) {
                        if(pplink==plink) {
                            plink_type.erase(this->_vecfind_ptr_itr<gNC::gLINK*>(plink_type, plink));
                        }
                    }
                }
                itrToErase.push_back(this->_find_ptr_itr<gNC::gLINK>(this->_links, plink));
            }
            for(auto itrs: itrToErase) {
                this->_links.erase(itrs);
            }
        }
    }
    else if(leaveFloating) {
        for(size_t i=0; i<4; i++) {
            const std::vector<gNC::gLINK*> linkVec = (
                i==0? NODE_toDelete->ln_in : 
                (i==1? NODE_toDelete->ln_out : 
                (i==2? NODE_toDelete->ln_add : 
                NODE_toDelete->ln_share))
            );

            for(gNC::gLINK* plink: linkVec) {
                if(NODE_toDelete==plink->src)       plink->src  = nullptr;
                else if(NODE_toDelete==plink->dest) plink->dest = nullptr;

            }
        }
    }

    this->_nodes.erase(this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toDelete));

    return 0;
}


int gNC::guiNodeChart::NODE_move(
    gNC::gNODE* NODE_toMove,
    float new_X,
    float new_Y,
    int moveMode
) {
    auto moveItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toMove);
    if(moveItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+_info_name+"NODE_move(gNC::gNODE*, float, float)"+
        " arg for gNC::gNODE address is invalid"
    );


    if(moveMode==0) {
        NODE_toMove->pos[0] = new_X;
        NODE_toMove->pos[1] = new_Y;
    }
    else if(moveMode==1) {
        NODE_toMove->pos[0] += new_X;
        NODE_toMove->pos[1] += new_Y;
    }
    else if(moveMode==-1) {}

    auto addNODE = [NODE_toMove](ImVec2 toChange) {
        return ImVec2(toChange.x+NODE_toMove->pos[0], toChange.y+NODE_toMove->pos[1]);
    };


    for(int v=0; v<4; v++) {
        std::vector<gNC::gLINK*>& checkVec = (
            v==0?  NODE_toMove->ln_in :
            (v==1? NODE_toMove->ln_out:
            (v==2? NODE_toMove->ln_add :
                   NODE_toMove->ln_share
            ))
        );

        for(gNC::gLINK* lnk: checkVec) {

            update_connect(
                lnk,
                (v==1 || v==3? addNODE(NODE_toMove->getConnectionPos(lnk->type_src)) : ImVec2(-1, -1)),
                (v==0 || v==2? addNODE(NODE_toMove->getConnectionPos(lnk->type_dest)): ImVec2(-1, -1)) 
            );
            ImVec2 connectPos(NODE_toMove->getConnectionPos((v==1||v==3? lnk->type_src : lnk->type_dest)));

            if(v==0 || v==2) { //in
                lnk->move_link(ImVec2(-2, -2), addNODE(connectPos));
                if(moveMode!=-1 && lnk->src) NODE_move(lnk->src,  0, 0, -1);
            }
            else if(v==1 || v==3) { //out
                lnk->move_link(addNODE(connectPos), ImVec2(-2, -2));
                if(moveMode!=-1 && lnk->dest) NODE_move(lnk->dest, 0, 0, -1);
            }
        }
    }


    return 0;
}

gNC::gLINK* gNC::guiNodeChart::LINK_create(
    size_t NODE_src_idx,
    size_t NODE_dest_idx,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc,
    ImVec2 pos_interm_src,
    ImVec2 pos_interm_dest
) {
    if(NODE_src_idx>=this->_nodes.size() || NODE_dest_idx>=this->_nodes.size()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(size_t, size_t, int, int, std::string, std::string):"+
            " arg for `{..}_idx` is out of range"
        );
    }

    gNC::gNODE *ptrSrc, *ptrDest;
    std::list<gNC::gNODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_src_idx);
    ptrSrc  = &*itr;
    itr = this->_nodes.begin();
    advance(itr, NODE_dest_idx);
    ptrDest = &*itr;

    return this->LINK_create(ptrSrc, ptrDest, type_src, type_dest, label, desc);
}
gNC::gLINK* gNC::guiNodeChart::LINK_create(
    gNC::gNODE* NODE_src,
    gNC::gNODE* NODE_dest,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc,
    ImVec2 pos_interm_src,
    ImVec2 pos_interm_dest
) {
    std::list<gNC::gNODE>::const_iterator
        checkItr_src    = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_src),
        checkItr_dest   = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_dest);

    if(checkItr_src==this->_nodes.end() || checkItr_dest==this->_nodes.end()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for node address(es) does not exist in stored nodes."
        );
    }
    if(searchVec<int>(std::vector<int>{1, 3, 5},type_src)==-1 && searchVec<int>(std::vector<int>{0, 2, 4},type_dest)==-1) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for type_{..} is/are not valid."
        );
    }


    this->_links.push_back(gNC::gLINK(type_src, type_dest, NODE_src, NODE_dest, label, desc));
    this->_lastAddedLink = &(this->_links.back());
    this->_lastAddedLink->addr = ptrToStr<gNC::gLINK*>(this->_lastAddedLink);


    if(type_src==1) _lastAddedLink->src->ln_out.push_back(_lastAddedLink);
    else            _lastAddedLink->src->ln_share.push_back(_lastAddedLink);

    if(type_dest==0)_lastAddedLink->dest->ln_in.push_back(_lastAddedLink);
    else            _lastAddedLink->dest->ln_add.push_back(_lastAddedLink);


    if((type_src==3 && type_dest==4) || (type_src==5 && type_dest==2)) {
        if(
            abs(NODE_src->pos.y+NODE_src->height - NODE_dest->pos.y) < _lastAddedLink->min__node ||
            abs(NODE_dest->pos.y+NODE_dest->height - NODE_src->pos.y) <_lastAddedLink->min__node
        ) {
            _lastAddedLink->type_dest = (type_src==3? 2 : 4);
            type_dest = _lastAddedLink->type_dest;
        }
    }

    ImVec2 srcPos  = NODE_src->getConnectionPos(type_src);
    ImVec2 destPos = NODE_dest->getConnectionPos(type_dest);


    update_connect(_lastAddedLink, srcPos, destPos);
    _lastAddedLink->move_link(
        add_nodePos(srcPos,  NODE_src),
        add_nodePos(destPos, NODE_dest)
    );
    
    NODE_move(NODE_src,  0, 0, -1);
    NODE_move(NODE_dest, 0, 0, -1);

    return this->_lastAddedLink;
}
gNC::gLINK* gNC::guiNodeChart::LINK_create_loose(
    ImVec2 loosePos,
    gNC::gNODE* _NODE,
    int type_NODE_connection,
    std::string label,
    std::string desc,
    ImVec2 pos_interm_NODE
) {
    std::list<gNC::gNODE>::const_iterator checkItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, _NODE);

    if(checkItr==this->_nodes.end()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create_loose(..):"+
            " arg for _NODE address is incorreect."
        );
    }
    if(searchVec<int>(std::vector<int>{1,2,3,4,5},type_NODE_connection)==-1) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create_loose(...):"+
            " arg for `type_NODE_connection`"
        );
    }

    int nType = 0; //`1`-src; `0`-dest
    if(searchVec<int>(std::vector<int>{1,3,5}, type_NODE_connection)!=-1) { //node is src (out, share)
        this->_links.push_back(gNC::gLINK(type_NODE_connection, 0, _NODE, nullptr, label, desc));
        nType = 1;
    }
    else { //node is dest (in, add)
        this->_links.push_back(gNC::gLINK(1, type_NODE_connection, nullptr, _NODE, label, desc));
        nType = 0;
    }

    this->_lastAddedLink = &(this->_links.back());
    this->_lastAddedLink->addr = ptrToStr<gNC::gLINK*>(this->_lastAddedLink);

    std::vector<gLINK*>& refVec = (
        type_NODE_connection==0?  _lastAddedLink->dest->ln_in :
        (type_NODE_connection==1? _lastAddedLink->src->ln_out :
        ((type_NODE_connection==2 || type_NODE_connection==4)? _lastAddedLink->dest->ln_add :
        _lastAddedLink->dest->ln_share))
    );
    refVec.push_back(_lastAddedLink);


    ImVec2 connecPos = _NODE->getConnectionPos(type_NODE_connection);

    if(nType==1)  {
        _lastAddedLink->Pos_src = add_nodePos(connecPos, _NODE); //ImVec2(_NODE->pos[0]+connecPos.x, _NODE->pos[1]+connecPos.y);
        _lastAddedLink->Pos_dest= loosePos;
        // _lastAddedLink->Pos_s1  = pos_interm_NODE;
    }
    else {
        _lastAddedLink->Pos_dest= add_nodePos(connecPos, _NODE); //ImVec2(_NODE->pos[0]+connecPos.x, _NODE->pos[1]+connecPos.y);
        _lastAddedLink->Pos_src = loosePos;
        // _lastAddedLink->Pos_d1  = pos_interm_NODE;
    }

    // ImVec2 pos_delta = ImVec2(_lastAddedLink->Pos_dest.x - _lastAddedLink->Pos_src.x, _lastAddedLink->Pos_dest.y - _lastAddedLink->Pos_src.y);
    // ImVec2 halfWayPoint = ImVec2(_lastAddedLink->Pos_src.x + pos_delta.x/2, _lastAddedLink->Pos_src.y + pos_delta.y/2);

    
    return this->_lastAddedLink;
}

int gNC::guiNodeChart::LINK_swapSrc(gNC::gLINK* toSwap, gNC::gNODE* newSrc, int srcType) {
    if(searchVec<int>(std::vector<int>{1,3,5}, srcType)==-1)
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): invalid `srcType` input");
    if(_find_ptr_itr<gNC::gNODE>(_nodes, newSrc)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `newSrc` is not a valid `gNC::gNODE` address");
    else if(_find_ptr_itr<gNC::gLINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `toSwap` is not a valid `gNC::gLINK` address");

    if(toSwap->src==newSrc) return 1;

    if(toSwap->src!=nullptr) {
        //locate and erase this link for the src NODE to swap from
        std::vector<gNC::gLINK*>& eraseVec = (toSwap->type_src==1? toSwap->src->ln_out : toSwap->src->ln_share);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }

    toSwap->src = newSrc;

    if(srcType==1) newSrc->ln_out.push_back(toSwap);
    else           newSrc->ln_share.push_back(toSwap);

    ImVec2 srcPos = newSrc->getConnectionPos(srcType);
    update_connect(toSwap, add_nodePos(srcPos, newSrc), ImVec2(-1, -1));
    toSwap->move_link(
        add_nodePos(srcPos, newSrc),
        // ImVec2(newSrc->pos[0]+srcPos.x, newSrc->pos[1]+srcPos.y),
        ImVec2(-2, -2)
    );

    return 0;
}
int gNC::guiNodeChart::LINK_swapDest(gNC::gLINK* toSwap, gNC::gNODE* newDest, int destType) {
    if(searchVec<int>(std::vector<int>{0,2,4}, destType)==-1)
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): invalid `destType` input");
    if(_find_ptr_itr<gNC::gNODE>(_nodes, newDest)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): arg for `newDest` is not a valid `NC::NODE` address");
    else if(_find_ptr_itr<gNC::gLINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): arg for `toSwap` is not a valid `NC::LINK` address");

    if(toSwap->dest==newDest) return 1;

    if(toSwap->dest!=nullptr) {
        // locate and erase the link address from the current dest NODE
        std::vector<gNC::gLINK*>& eraseVec = (toSwap->type_dest==0? toSwap->dest->ln_in : toSwap->dest->ln_add);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }
    // update dest address in the link
    toSwap->dest = newDest;

    // add NC::LINK address to the new NODE in correct std::vector container
    if(destType==0) newDest->ln_in.push_back(toSwap);
    else            newDest->ln_add.push_back(toSwap);


    ImVec2 destPos = newDest->getConnectionPos(destType);
    update_connect(toSwap, ImVec2(-1, -1), add_nodePos(destPos, newDest));
    toSwap->move_link(
        ImVec2(-2, -2),
        add_nodePos(destPos, newDest)
        // ImVec2(newDest->pos[0]+destPos.x, newDest->pos[1]+destPos.y)
    );

    return 0;
}
int gNC::guiNodeChart::LINK_delete(gNC::gLINK* LINK_toDelete) {
    std::list<gNC::gLINK>::const_iterator linkItr = _find_ptr_itr<gNC::gLINK>(this->_links, LINK_toDelete);
    if(linkItr==this->_links.end()) std::runtime_error("ERROR: "+this->_info_name+"LINK_delete(gNC::gLINK*): invalid `gNC::gLINK` address to delete");


    if(LINK_toDelete->src!=nullptr) {
        std::vector<gNC::gLINK*>& eraseVec = (LINK_toDelete->type_src==1? LINK_toDelete->src->ln_out : LINK_toDelete->src->ln_share);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }
    if(LINK_toDelete->dest!=nullptr) {
        std::vector<gNC::gLINK*>& eraseVec = (LINK_toDelete->type_dest==0? LINK_toDelete->dest->ln_in : LINK_toDelete->dest->ln_add);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }

    this->_links.erase(linkItr);

    return 0;
}



bool _draw__node_cosmetics(
    std::list<gNC::gNODE>::iterator itr,
    ImVec2 nodePos,
    ImDrawList* win_draw_list
) {
    bool result = true;
    static std::vector<int> buttons(4, 0);

    ImDrawList* local_drawList = ImGui::GetWindowDrawList();

    ImGui::TextUnformatted((*itr).label.c_str());
    ImGui::Separator();
    ImGui::TextUnformatted((*itr).desc.c_str());
    ImGui::Separator();
    ImGui::TextWrapped((*itr).bodyText.c_str());

    (*itr).draw_connection(std::vector<ImDrawList*>{local_drawList, win_draw_list}, nodePos);

    return result;
}


int _draw_NODEcheckConnects(
    std::list<gNC::gNODE>::iterator _node,
    ImVec2 nodePos
) {
    ImGuiIO& io = ImGui::GetIO();
    int result = -1;

    ImVec2 attachROI = (*_node).ROI_attach;

    for(int i=0; i<6; i++) {
        ImVec2 tempPos = (
            i==0? (*_node).pos_in :
            (i==1? (*_node).pos_out :
            (i==2? (*_node).pos_add_0 :
            (i==3? (*_node).pos_share_0 :
            (i==4? (*_node).pos_add_1 :
            (*_node).pos_share_1))))
        );
        if(inRegion(
            io.MousePos,
            ImVec2(nodePos.x + tempPos.x - attachROI.x/2, nodePos.y + tempPos.y - attachROI.y/2),
            ImVec2(nodePos.x + tempPos.x + attachROI.x/2, nodePos.y + tempPos.y + attachROI.y/2)
        )) {
            result = i;
            break;
        }
    }


    return result;
}

int gNC::guiNodeChart::draw() {
    static bool local_init = false;
    static std::vector<std::vector<int>>* pressed_keys;

    if(!local_init) pressed_keys = update_keys();


    ImGuiIO& io = ImGui::GetIO(); //(void)io;


    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    #if _DEBUG
    std::cout<< "{"<<std::setw(2)<< mouseAction_left<<","<<std::setw(3)<<decay_mouseClick_left<<"} ";
    #endif

    /// @brief whether a node has been focused when they're iterated.
    bool node_focused = false;
    /// @brief whether a link has been focused when they're iterated.
    bool link_focused = false;

    static gLINK* focused_link{nullptr};


    for(auto itr=_links.begin(); itr!=this->_links.end(); ++itr) {
        std::vector<ImVec2> linkPos{
            ImVec2((*itr).Pos_src.x  + screen_pos[0], (*itr).Pos_src.y  + screen_pos[1]),
            ImVec2((*itr).Pos_dest.x + screen_pos[0], (*itr).Pos_dest.y + screen_pos[1])
        };
        if(
            (linkPos[0].x < 0 && linkPos[1].x < 0) || (linkPos[0].x > screen_dim[0] && linkPos[1].x > screen_dim[0]) ||
            (linkPos[0].y < 0 && linkPos[1].y < 0) || (linkPos[0].y > screen_dim[1] && linkPos[1].y > screen_dim[1])
        ) continue;
        
        if((*itr).region(
            io.MousePos,
            ImVec2(screen_pos[0], screen_pos[1])
        )) {
            
            if(
                isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-1]) &&
                !isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-2])
                // !_winFocused__node_details
            ) {

                if(mouseAction_left==3 || mouseAction_left==-1) {
                    (*itr).draw__state = 2;

                    link_focused = true;
                    focused_link = &(*itr);
                    linkPtr_menu__link_details = &(*itr);
                    mouseAction_left = 3;
                    decay_mouseClick_left = 100;
                }
            }
            else {
                (*itr).draw__state = 1;

            }
        }
        else {
            (*itr).draw__state = 0;
        }

        // if(
        //     // !_winFocused__link_details &&
        //     // !_winFocused__node_details &&
        //     link_focused && focused_link == &(*itr)
        // ) {
        //     linkPtr_menu__link_details = &(*itr);
        // }


        (*itr).draw_link(std::vector<ImDrawList*>{draw_list}, ImVec2(screen_pos[0], screen_pos[1]));
    }
    for(auto itr=_nodes.begin(); itr!=this->_nodes.end(); ++itr) {
        ImVec2 nodePos = ImVec2((*itr).pos[0] + screen_pos[0], (*itr).pos[1] + screen_pos[1]);

        if((nodePos.x + (*itr).width  < 0  || nodePos.x > screen_dim[0]) || (nodePos.y + (*itr).height < 0  || nodePos.y > screen_dim[1])) continue;

        ImGuiWindowFlags win_flags = 0;
        win_flags |= ImGuiWindowFlags_NoResize;
        win_flags |= ImGuiWindowFlags_NoCollapse;
        win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
        win_flags |= ImGuiWindowFlags_NoMove;
        

        int node_connect = _draw_NODEcheckConnects(itr, nodePos);


        if(isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-1])) {
            if(node_connect==-1) {
                if(inRegion(io.MousePos, nodePos, ImVec2(nodePos.x+(*itr).width, nodePos.y+(*itr).height))) { //region: Node
                    if(mouseAction_left==1 || mouseAction_left==-1) {
                        lockMove_node = false;
                        mouseAction_left = 1;
                        decay_mouseClick_left = 100;
                    }
                }
            }
            else if(node_connect!=-1) { //region: Node connection
                if(mouseAction_left==2 || mouseAction_left==-1) {
                    (*itr).state_connections[node_connect] = 2;
                    if(mouseAction_left!=2) { //mouseAction is performed in this program iteration
                        // dragConnectCreate_tempLink = gNC::gLINK();
                        dragConnectCreate_tempLink._init = true;
                        if(node_connect%2==0) { //started at dest node point
                            dragConnectCreate_tempLink.dest = &(*itr);
                            dragConnectCreate_tempLink.type_dest= node_connect;
                            dragConnectCreate_tempLink.Pos_dest  = add_nodePos((*itr).getConnectionPos(node_connect), &(*itr));
                            dragConnectCreate_startedEnd = 0;

                        }
                        else { //started at src node point
                            dragConnectCreate_tempLink.src  = &(*itr);
                            dragConnectCreate_tempLink.type_src = node_connect;
                            dragConnectCreate_tempLink.Pos_src = add_nodePos((*itr).getConnectionPos(node_connect), &(*itr));
                            dragConnectCreate_startedEnd = 1;
                        }
                    }
                    mouseAction_left = 2;
                    decay_mouseClick_left = 100;
                    if(nodePtr__dragConnectCreate_start != &(*itr)) nodePtr__dragConnectCreate_start = &(*itr);
                }
            }
        }
        else {
            if(node_connect!=-1) {
                (*itr).state_connections[node_connect] = 1;
                if(
                    static_mouseAction_left==2 && dragConnectCreate_tempLink._init
                ) {
                    //form a proper new link if node_connect is not the same as prev and node address is new

                    if((dragConnectCreate_startedEnd==1? dragConnectCreate_tempLink.src : dragConnectCreate_tempLink.dest) != &(*itr)) {
                        if(
                            !option_dragConnectCreate_tempLink_snapIn &&
                            (dragConnectCreate_startedEnd==1? node_connect%2==0 : node_connect%2!=0)
                        ) {
                            if(dragConnectCreate_startedEnd==0) {
                                dragConnectCreate_tempLink.src = &(*itr);
                                dragConnectCreate_tempLink.type_src = node_connect;
                            }
                            else {
                                dragConnectCreate_tempLink.dest = &(*itr);
                                dragConnectCreate_tempLink.type_dest = node_connect;
                            }

                            this->LINK_create(
                                dragConnectCreate_tempLink.src,
                                dragConnectCreate_tempLink.dest,
                                dragConnectCreate_tempLink.type_src,
                                dragConnectCreate_tempLink.type_dest
                            );
                            dragConnectCreate_tempLink = gNC::gLINK();
                        }
                    }
                }
            }
            else {

            }
            
            if(mouseAction_left!=2) {
                for(int i=0; i<6; i++) if((*itr).state_connections[i]!=0) { (*itr).state_connections[i]=0; }
            }

        }

        if(isKeyPressed(656, &(*pressed_keys)[pressed_keys->size()-1])) {
            if(node_connect==-1) {
                if(inRegion(io.MousePos, nodePos, ImVec2(nodePos.x+(*itr).width, nodePos.y+(*itr).height))) {
                    if(mouseAction_right==1 || mouseAction_right==-1) {
                        mouseAction_right = 1;
                        nodePtr_menu__rightClick = &(*itr);
                        decay_mouseClick_right = 100;
                    }
                }
            }
            else if(node_connect!=-1) {
                if(mouseAction_right==2 || mouseAction_right==-1) {
                    (*itr).state_connections[node_connect] = 2;
                    mouseAction_right = 2;
                    decay_mouseClick_right = 100;
                }
            }
        }
        else {
            if(node_connect!=-1) { (*itr).state_connections[node_connect] = 1; }
            else if(mouseAction_right!=2) { for(int i=0; i<6; i++) {if((*itr).state_connections[i]!=0) (*itr).state_connections[i]=0;} }
        }


        ImGui::Begin((*itr).addr.c_str(), NULL, win_flags);
        ImGui::SetWindowSize(ImVec2(((*itr).width), (*itr).height));

        if(!(*itr).init) {
            ImGui::SetWindowPos(ImVec2((*itr).pos[0], (*itr).pos[1]));
            (*itr).init = true;
        }


        if(ImGui::IsWindowFocused() || !local_init) {
            // _menu__node_details(&(*itr));
            nodePtr_menu__node_details  = &(*itr);
            node_focused   = true;
            if(!lockMove_node && ((*pressed_keys)[pressed_keys->size()-1]).size() > 0 && isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-1])) {
                NODE_move(&(*itr), io.MouseDelta.x, io.MouseDelta.y, 1);
                ImGui::SetWindowPos(ImVec2((*itr).pos[0] + screen_pos[0], (*itr).pos[1] + screen_pos[1]));
            }
        }
        else {
            ImGui::SetWindowPos(nodePos);
        }
        _draw__node_cosmetics(itr, nodePos, draw_list);

        ImGui::End();
    }



    // std::cout <<"winFocus: ["<< (nodePtr_menu__node_details? ptrToStr<gNC::gNODE*>(nodePtr_menu__node_details) : "nullptr")<<", "<< (linkPtr_menu__link_details? ptrToStr<gNC::gLINK*>(linkPtr_menu__link_details) : "nullptr")<<"]";
    // std::cout <<std::boolalpha<<" "<<link_focused<<" ";
    // std::cout <<"mouseAction_left:"<<mouseAction_left;

    if(nodePtr_menu__node_details) _menu__node_details(nodePtr_menu__node_details);
    if(linkPtr_menu__link_details) _menu__link_details(linkPtr_menu__link_details);

    if(static_mouseAction_left!=1 && (mouseDrag_left)) {
        if(!node_focused) nodePtr_menu__node_details = nullptr;
    }
    if(static_mouseAction_left!=3 && (mouseDrag_left)) {
        if(!link_focused) linkPtr_menu__link_details = nullptr;

    }

    if((mouseAction_left==0 || mouseAction_left==-1) && isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-1])) {
        lockMove_screen = false;
        mouseAction_left = 0;
        decay_mouseClick_left = 100;
        if(mouseDrag_left) focused_link = nullptr;
        #if _DEBUG
        std::cout << " [screen]";
        #endif
    }
    if(mouseAction_left!=-1) {
        if(decay_mouseClick_left>0) {
            if(!isKeyPressed(655, &(*pressed_keys)[pressed_keys->size()-1])) decay_mouseClick_left -= mouseTimer_decay;
        }
        else {
            decay_mouseClick_left = 0;
            mouseAction_left = -1;
        }
    }

    if(mouseAction_left!=-1) static_mouseAction_left = mouseAction_left;
    if(mouseAction_left!=0 || mouseAction_left==2) lockMove_screen  = true;
    if(mouseAction_left!=1 || mouseAction_left==2) lockMove_node    = true;

    if(isKeyPressed(656, &(*pressed_keys)[pressed_keys->size()-1])) {
        if(mouseAction_right==0 || mouseAction_right==-1) {
            ImGui::OpenPopup("_menu__rightClick__default");
            // mouseAction_right = 0;
            mouseAction_right = 0;
        }
        else if(mouseAction_right==1) {
            ImGui::OpenPopup("_menu__rightClick__node");
            // mouseAction_right =
        }
    }
    _menu__rightClick(thisPtr);

    // std::cout << mouseAction_left << " " << static_mouseAction_left << std::endl;

    mouseAction_right = -1;
    // nodePtr_menu__rightClick = nullptr;
    
    // std::cout << std::endl;

    if(!local_init) local_init = true;
    return 0;
}

int gNC::guiNodeChart::save(
    std::string filename,
    bool overwrite
) {


    return 0;
}

