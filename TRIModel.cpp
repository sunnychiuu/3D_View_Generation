#include <glm/glm.hpp>
#include "TRIModel.h"

using namespace std;
using namespace glm;

bool TRIModel::loadFromFile(const char* fileName){
	char tmpstr[100] = "";
	double max[3]={-DBL_MAX, -DBL_MAX, -DBL_MAX};
	double min[3]={DBL_MAX, DBL_MAX, DBL_MAX};
	FILE* inFile = fopen(fileName, "r");
	if(!inFile){
		cout << "Can not open object File \"" << fileName << "\" !" << endl;
		return false;
	}

	cout <<"Loading \"" << fileName << "\" !" << endl;
	while(fscanf(inFile,"%s",tmpstr) != EOF){
		vec3 tmpvert;
		vec3 tmpn;
		int tmpint[6];
		vec3 fcolor;
		vec3 bcolor;
		fscanf(inFile,"%d %d %d %d %d %d",&tmpint[0], &tmpint[1], &tmpint[2], &tmpint[3], &tmpint[4], &tmpint[5]);
		for(int i = 0; i < 3; ++i){
			fcolor[i] = tmpint[i] / 255.0;
			bcolor[i] = tmpint[i+3] / 255.0;
		}
		for(int i = 0; i < 3; i++){
			fscanf(inFile,"%f %f %f %f %f %f",&tmpvert.x,&tmpvert.y, &tmpvert.z, &tmpn.x, &tmpn.y, &tmpn.z);
			vertices.push_back(tmpvert);
			normals.push_back(tmpn);
			forecolors.push_back(fcolor);
			backcolors.push_back(bcolor);
			for(int j = 0; j < 3; j++){
				if(tmpvert[j] < min[j]){
					min[j] = tmpvert[j];
				}
				if(tmpvert[j] > max[j]){
					max[j] = tmpvert[j];
				}
			}
		}
	}
	for(int i = 0; i < 3; i++)
		center[i] = (min[i] + max[i]) / 2;
	return true;
}


TRIModel::TRIModel(){
}

TRIModel::~TRIModel(){
}

bool OFFModel::loadFromFile(const char* fileName){
	char tmpstr[100] = "";
    vector<vec3> vertices_;
    vector<int> vertInds;
    vector<vec3> faceNormals;
    vector<vector<int> > faceOfVertexs;
    int nVertex, nFace, nEdge;
	double max[3]={-DBL_MAX, -DBL_MAX, -DBL_MAX};
	double min[3]={DBL_MAX, DBL_MAX, DBL_MAX};

	FILE* inFile = fopen(fileName, "r");
	if(!inFile){
		cout << "Can not open off File \"" << fileName << "\" !" << endl;
		return false;
	}

	cout <<"Loading (3) \"" << fileName << "\" !" << endl;
    fscanf(inFile, "%s", tmpstr);
    fscanf(inFile, "%d %d %d", &nVertex, &nFace, &nEdge);

    vertices_.resize(nVertex);
    faceNormals.resize(nFace);
    faceOfVertexs.resize(nVertex);

    // read vertices
    for (int i = 0; i < nVertex; i++) {
        vec3 tmpvert;
        fscanf(inFile, "%f %f %f", &tmpvert.x, &tmpvert.y, &tmpvert.z);
        vertices_[i] = tmpvert;

        for(int j = 0; j < 3; j++){
            if (tmpvert[j] < min[j]){
                min[j] = tmpvert[j];
            }
            if (tmpvert[j] > max[j]){
                max[j] = tmpvert[j];
            }
        }
    }

    // read faces
    for (int i = 0; i < nFace; i++) {
        int nFaceVert;
        int vertInd[3];
        vec3 faceNormal;

        fscanf(inFile, "%d %d %d %d", &nFaceVert, &vertInd[0], &vertInd[1], &vertInd[2]);

        for (int j = 0; j < 3; j++) {
            this->vertices.push_back(vertices_[vertInd[j]]);
            this->backcolors.push_back(vec3(0.3, 0.3, 0.3));
            this->forecolors.push_back(vec3(0.3, 0.3, 0.3));
            vertInds.push_back(vertInd[j]);
        }

        // calc face normal
        vec3 edge1, edge2;
        edge1 = vertices_[vertInd[0]] - vertices_[vertInd[1]];
        edge2 = vertices_[vertInd[0]] - vertices_[vertInd[2]];
        faceNormals[i] = normalize(cross(edge1, edge2));
        
        // store the face index for all vertices
        for (int j = 0; j < 3; j++)
            faceOfVertexs[vertInd[j]].push_back(i);
    }

    // do vertex normals by averaging face normals
    for (int i = 0; i < vertInds.size(); i++) {
        vector<int> useFaceInds;
        vec3 totalNormal(0.0, 0.0, 0.0);

        useFaceInds = faceOfVertexs[vertInds[i]];

        for (int j = 0; j < useFaceInds.size(); j++) {
            vec3 normal;
            normal = faceNormals[useFaceInds[j]];
            totalNormal += normal;
        }
        totalNormal = normalize(totalNormal);
        this->normals.push_back(totalNormal);
    }

	for (int i = 0; i < 3; i++)
		center[i] = (min[i] + max[i]) / 2;

    return true;
}


OFFModel::OFFModel(){
}

OFFModel::~OFFModel(){
}

/*bool OBJModel::loadFromFile(const char* fileName){
    char tmpstr[100] = "";
    vector<vec3> vertices_;
    vector<int> vertInds;
    vector<vec3> faceNormals;
    vector<vector<int> > faceOfVertexs;
    int nVertex, nFace, nEdge;
    char lineHeader[1000];
    double max[3]={-DBL_MAX, -DBL_MAX, -DBL_MAX};
    double min[3]={DBL_MAX, DBL_MAX, DBL_MAX};

    FILE* inFile = fopen(fileName, "r");
    if(!inFile){
        cout << "Can not open off File \"" << fileName << "\" !" << endl;
        return false;
    }

    cout <<"Loading obj  \"" << fileName << "\" !" << endl;

    vector <int> vertInd;
    //int vertInd[3];
    vec3 faceNormal;
    vec3 tmpvert;
    char* tmpv;
    int tmpv_int;
    char tmpvertInd[1000];
    char lineContent[1000];
    char* pLineContent;
    int i=0,len;
    while (fscanf(inFile, "%s", lineHeader) != EOF) {
        //printf("*\n");
        // read vertex
        if (strcmp(lineHeader, "v") == 0) {
            fscanf(inFile, "%f %f %f", &tmpvert.x, &tmpvert.y, &tmpvert.z);
            faceOfVertexs.push_back(vector<int>());
            vertices_.push_back(tmpvert);
            for(int j = 0; j < 3; j++){
                if (tmpvert[j] < min[j]){
                    min[j] = tmpvert[j];
                }
                if (tmpvert[j] > max[j]){
                    max[j] = tmpvert[j];
                }
            }
        }
        //read face
        else if(strcmp(lineHeader, "f") == 0) {
            fgets(lineContent, sizeof(lineContent), inFile);
            pLineContent = lineContent;

            // use strtok to seperate the sub-strings
            while(sscanf(pLineContent, "%s%n", tmpvertInd, &len) == 1)
            {
                //printf("%s [len = %d]\n",tmpvertInd, len);
                tmpv = strtok(tmpvertInd,"/");
                tmpv_int = atoi(tmpv) - 1;
                vertInd.push_back(tmpv_int); 
                pLineContent += len;

            }
            //printf("%d %d %d \n",vertInd[0],vertInd[1],vertInd[2]);
            for(int j = 0; j < 3; j++)
            {
                this->vertices.push_back(vertices_[vertInd[j]]);
                //this->vertices.push_back(vertices_[vertInd[j] - 1]);
                this->backcolors.push_back(vec3(0.3, 0.3, 0.3));
                this->forecolors.push_back(vec3(0.3, 0.3, 0.3));
                vertInds.push_back(vertInd[j]);               
            }

            // read faces
              else if (strcmp(lineHeader, "f") == 0) {
              fscanf(inFile, "%d %d %d", &vertInd[0], &vertInd[1], &vertInd[2]);
              printf("%d %d %d \n",vertInd[0],vertInd[1],vertInd[2]);
              for (int j = 0; j < 3; j++) {
              this->vertices.push_back(vertices_[vertInd[j] - 1]);
              this->backcolors.push_back(vec3(0.3, 0.3, 0.3));
              this->forecolors.push_back(vec3(0.3, 0.3, 0.3));
              vertInds.push_back(vertInd[j]);
              }
            // calc face normal

            vec3 edge1, edge2;
            edge1 = vertices_[vertInd[0]] - vertices_[vertInd[1]];
            edge2 = vertices_[vertInd[0]] - vertices_[vertInd[2]];
            faceNormals.push_back(normalize(cross(edge1, edge2)));
            //cout << vertices_[vertInd[0]]<<" "<<vertices_[vertInd[1]]<<" "<<vertices_[vertInd[2]]<< " facenormal " << normalize(cross(edge1, edge2))<<endl;

            // store the face index for all vertices
            for (int j = 0; j < 3; j++)
            {
                assert (j < vertInds.size());
                faceOfVertexs[vertInd[j]].push_back(i);
            }
            i++;
            vertInd.clear();
        }

        //fscanf(inFile, "%s", tmpstr);
        //fscanf(inFile, "%d %d %d", &nVertex, &nFace, &nEdge);
        }
        printf("finish part1\n");
        // do vertex normals by averaging face normals
        for (int i = 0; i < vertInds.size(); i++) {
            vector<int> useFaceInds;
            vec3 totalNormal(0.0, 0.0, 0.0);

            useFaceInds = faceOfVertexs[vertInds[i]];

            for (int j = 0; j < useFaceInds.size(); j++) {
                vec3 normal;
                normal = faceNormals[useFaceInds[j]];
                totalNormal += normal;
            }
            totalNormal = normalize(totalNormal);
            //cout << "totalNormal "<<vertInds[i]<<" "<<totalNormal.x<<" "<<totalNormal.y<<" "<<totalNormal.z<<endl;
            this->normals.push_back(totalNormal);
        }
        printf("finish part2\n");

        for (int i = 0; i < 3; i++)
            center[i] = (min[i] + max[i]) / 2;
        printf("function finish\n");
        return true;
    }
*/
