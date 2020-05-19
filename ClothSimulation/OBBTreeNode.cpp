#include "OBBTreeNode.h"
float OBBTreeNode::thick = 0.02f;
float OBBTreeNode::reflectW = 0.2f;
OBBTreeNode::OBBTreeNode()
{
}

OBBTreeNode::OBBTreeNode(Cloth* cloth)
{
    new(this) OBBTreeNode(cloth,0,0,cloth->height-2,0,cloth->width-2);
}

OBBTreeNode::OBBTreeNode(Cloth* cloth, int way, int h_start, int h_end, int w_start, int w_end)
{
    if (h_start == h_end && w_start == w_end) {
        new(this) OBBTreeNode(&(cloth->node[h_start][w_start]),& (cloth->node[h_start + 1][w_start]),& (cloth->node[h_start][w_start + 1]), &(cloth->node[h_start + 1][w_start + 1]));
        if ((h_start == cloth->height - 2) && (w_start == cloth->width - 2)) {
            for (int i = 0; i < 4;i++) {
                bound[i] = true;
            }
            return;
        }
        if (h_start == cloth->height - 2) {
            bound[1] = true;
            return;
        }
        if (w_start == cloth->width - 2) {
            bound[2] = true;
            return;
        }
        return;
    }
    if (h_start == h_end && way == 0)
        way = 1;
    if (w_start == w_end && way == 1)
        way = 0;
    if (way == 0) {
        OBBTreeNode* ON1 = new OBBTreeNode(cloth, 1-way, h_start, (h_end+h_start)/2, w_start, w_end);
        OBBTreeNode* ON2 = new OBBTreeNode(cloth, 1-way, (h_end + h_start) / 2+1, h_end, w_start, w_end);
        new(this) OBBTreeNode(ON1,ON2);
        return;
    }
    if (way == 1) {
        OBBTreeNode* ON2 = new OBBTreeNode(cloth, 1 - way, h_start, h_end, w_start, (w_start+w_end)/2);
        OBBTreeNode* ON1 = new OBBTreeNode(cloth, 1 - way, h_start, h_end , (w_start+w_end)/2+1, w_end);
        new(this) OBBTreeNode(ON1, ON2);
        return;
    }
}

OBBTreeNode::OBBTreeNode(Particle* p1, Particle* p2, Particle* p3, Particle* p4)
{
    isleaf = true;
    node.push_back(p1);
    node.push_back(p2);
    node.push_back(p3);
    node.push_back(p4);
    
}

OBBTreeNode::OBBTreeNode(OBBTreeNode* OBB1, OBBTreeNode* OBB2)
{
    isleaf = false;
    child.push_back(OBB1);
    child.push_back(OBB2);
    
}

vector<vec3> OBBTreeNode::OBBxyz(vector<vec3> nodelist)
{
    Eigen::MatrixXf input(nodelist.size(), 3);

    for (int i = 0; i < nodelist.size(); i++) {
        input(i,0)=nodelist[i].x;
        input(i,1)= nodelist[i].y;
        input(i,2)= nodelist[i].z;
    }
    Eigen::MatrixXf meanVec = input.colwise().mean();
    Eigen::RowVectorXf meanVecRow(Eigen::RowVectorXf::Map(meanVec.data(), input.cols()));
    Eigen::MatrixXf zeroMeanMat = input;
    zeroMeanMat.rowwise() -= meanVecRow;
    //std::cout << zeroMeanMat << std::endl;
    Eigen::MatrixXf covMat;
    if (input.rows() == 1)
        covMat = (zeroMeanMat.transpose() * zeroMeanMat) / double(input.rows());
    else {
        covMat = (zeroMeanMat.transpose() * zeroMeanMat) / double(input.rows() - 1);
        //std::cout << covMat << std::endl;
    }
    Eigen::SelfAdjointEigenSolver <Eigen::MatrixXf> ES(covMat);
   //std::cout << ES.eigenvectors() << std::endl;
   // std::cout << ES.eigenvalues() << std::endl;
    Eigen::MatrixXf ev = ES.eigenvectors();
     vector<vec3> output;
     for (int i = 0; i < 3; i++) {
         output.push_back(vec3(ev(0, 2 - i), ev(1, 2 - i), ev(2, 2 - i)));
     }
    return output;
}

void OBBTreeNode::self_collision()
{
    for (int i = 0; i < child.size(); i++) {
        for (int j = i + 1; j < child.size(); j++) {
            collition(child[i], child[j]);
        }
    }
    for (int i = 0; i < child.size(); i++)
        child[i]->self_collision();
}

void OBBTreeNode::update()
{
    if (isleaf) {
        vector<vec3> tep;
        tep.push_back(node[0]->pos);
        tep.push_back(node[1]->pos);
        tep.push_back(node[2]->pos);
        tep.push_back(node[3]->pos);
        vector<vec3> direction;
        direction = OBBxyz(tep);
        for (int i = 0; i < 3; i++) {
            dir[i] = direction[i];
        }
        for (int i = 0; i < 3; i++) {
            bool minfirst = true;
            bool maxfirst = true;
            float minvalue = 0;
            float maxvalue = 0;
            for (int j = 0; j < tep.size(); j++) {
                float pro_value = glm::dot(tep[j], dir[i]);
                if (minfirst || pro_value < minvalue) {
                    minfirst = false;
                    minvalue = pro_value;
                }
                if (maxfirst || pro_value > maxvalue) {
                    maxfirst = false;
                    maxvalue = pro_value;
                }
            }
            center[i] = (maxvalue + minvalue) / 2;
            dis[i] = (maxvalue - minvalue) / 2;
        }
        center = center[0] * dir[0] + center[1] * dir[1] + center[2] * dir[2];
        dis[2] += thick;
        return;
    }

    for (int i = 0; i < child.size(); i++) {
        child[i]->update();
    }
    vector<vec3> tep;
    for (int i = 0; i < child.size(); i++) {
        OBBTreeNode* OBBt = child[i];
        float mapi[2] = { -1,1 };
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vec3 tepvec3 = OBBt->center + mapi[i] * OBBt->dis[0]*OBBt->dir[0] + mapi[j]*OBBt->dis[1] * OBBt->dir[1] + mapi[k]*OBBt->dis[2] * OBBt->dir[2];
                    tep.push_back(tepvec3);
                }
            }
        }
    }
   
    vector<vec3> direction;
    direction = OBBxyz(tep);
    for (int i = 0; i < 3; i++) {
        dir[i] = direction[i];
    }
    for (int i = 0; i < 3; i++) {
        bool minfirst = true;
        bool maxfirst = true;
        float minvalue = 0;
        float maxvalue = 0;
        for (int j = 0; j < tep.size(); j++) {
            float pro_value = glm::dot(tep[j], dir[i]);
            if (minfirst || pro_value < minvalue) {
                minfirst = false;
                minvalue = pro_value;
            }
            if (maxfirst || pro_value > maxvalue) {
                maxfirst = false;
                maxvalue = pro_value;
            }
        }
        center[i] = (maxvalue + minvalue) / 2;
        dis[i] = (maxvalue - minvalue) / 2;
    }
    center = center[0] * dir[0] + center[1] * dir[1] + center[2] * dir[2];
}

void OBBTreeNode::collition(OBBTreeNode* N1, OBBTreeNode* N2)
{
    //if (!(N1->isleaf)) {
    if (!intersect(N1, N2)) {
        return;
    }
    if ((N1->isleaf) && (N2->isleaf)) {
    //  std::cout << "two_leaf" << std::endl;
        for (int outi = 0; outi < 2; outi++) {
            for (int i = 0; i <= N1->node.size() - 3; i++) {
                vec3 base1 = N1->node[i + 1]->pos - N1->node[i]->pos;
                vec3 base2 = N1->node[i + 2]->pos - N1->node[i]->pos;
                vec3 basen = glm::normalize(glm::cross(base1, base2));
                glm::mat3 trans = glm::inverse(glm::mat3(base1, base2, basen));
                for (int j = 0; j < N2->node.size(); j++) {
                    if (N2->bound[j]) {
                        bool self = false;
                        for (int k = 0; k < 3; k++) {
                            if (N2->node[j] == N1->node[i + k]) {
                                self = true;
                                break;
                            }
                        }
                        if (self) continue;
                        vec3 newcor = trans * (N2->node[j]->pos - N1->node[i]->pos);
                        if (newcor.x + newcor.y > 0 && newcor.x + newcor.y < 1 &&newcor.x>0&&newcor.y>0) {
                            vec3 Vave = (N1->node[i]->velocity + N1->node[i + 1]->velocity + N1->node[i + 2]->velocity) / 3.0f;
                            float Vz = glm::dot(basen, N2->node[j]->velocity - Vave);
                            if (abs(newcor.z) < thick) {
                                if (newcor.z * Vz <=0) {
                                    N2->node[j]->velocity = N2->node[j]->velocity - Vz * basen;
                                    if (newcor.z < 0) {
                                        N2->node[j]->pos = N2->node[j]->pos - newcor.z * basen - thick * basen;
                                    }
                                    else {
                                        N2->node[j]->pos = N2->node[j]->pos - newcor.z * basen + thick * basen;
                                       }
                                    N1->node[i ]->velocity += 1.0f / 3 * Vz * basen;
                                    N1->node[i + 1]->velocity += 1.0f / 3 * Vz * basen;
                                    N1->node[i + 2]->velocity += 1.0f / 3 * Vz * basen;
                                }
                          //      std::cout <<  N2->node[j]->pos.z << std::endl;

                            }
                        }
                    }
                }
            }
            OBBTreeNode* t = N1;
            N1 = N2;
            N2 = t;
        }
        return;
    }
    if ((N1->isleaf) && (!N2->isleaf)) {
        for (int i = 0; i < N2->child.size(); i++) {
            collition(N1, N2->child[i]);
      }
        return;
    }
    if (!(N1->isleaf) && (!N2->isleaf)) {
        for (int i = 0; i < N1->child.size(); i++) {
            collition(N1->child[i],N2);
        }
        return;
    }
}

bool OBBTreeNode::intersect(OBBTreeNode* N1, OBBTreeNode* N2)
{
    vector<vec3> tep;
        float mapi[2] = { -1,1 };
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vec3 tepvec3 = N2->center + N2->dis[0]*mapi[i] *N2->dir[0] + N2->dis[1]*mapi[j] * N2->dir[1] + N2->dis[2]*mapi[k] * N2->dir[2];
                    tep.push_back(tepvec3);
                }
            }
        }
        for (int i = 0; i < 3; i++) {
            int where = -1;
            bool sepflag = true;
            for (int j = 0; j < tep.size(); j++) {
                float pro_value = glm::dot(N1->dir[i], tep[j]-N1->center);
                if (where == -1) {
                    if (pro_value >= -N1->dis[i] && pro_value <= N1->dis[i]) {
                        sepflag = false;
                        break;
                    }
                    if (pro_value < -N1->dis[i]) {
                        where = 0;
                    }
                    if (pro_value > N1->dis[i]) {
                        where = 1;
                    }
                }
                if (where == 0) {
                    if (pro_value >= -N1->dis[i]) {
                        sepflag = false;
                        break;
                    }
                }
                if (where == 1) {
                    if (pro_value <= N1->dis[i]) {
                        sepflag = false;
                        break;
                    }
                }
            }
            if (sepflag) {
                return false;
            }
        }

        return true;
}



