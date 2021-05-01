///////////////////////////////////////////////////////////////////////////////
///  GraphElements.h
///  <TODO: insert file description here>
///
///  @remarks <TODO: insert remarks here>
///
///  @author Yan Qi @date 5/28/2010
///  @author Cytroyd @date 4/26/2021
///  $Id: GraphElements.h 65 2010-09-08 06:48:36Z yan.qi.asu $
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <iostream>
using namespace std;
template<class T>
class WeightGreater
{
public:
    // Determine priority.
    bool operator()(const T& a, const T& b) const
    {
        return a.Weight() > b.Weight();
    }

    bool operator()(const T* a, const T* b) const
    {
        return a->Weight() > b->Weight();
    }
};

template<class T>
class WeightLess
{
public:
    // Determine priority.
    bool operator()(const T& a, const T& b) const
    {
        return a.Weight() < b.Weight();
    }

    bool operator()(const T* a, const T* b) const
    {
        return a->Weight() < b->Weight();
    }
};

//////////////////////////////////////////////////////////////////////////
// A class for the object deletion
//////////////////////////////////////////////////////////////////////////
template<class T>
class DeleteFunc
{
public:
    void operator()(const T* it) const
    {
        delete it;
    }
};



/**************************************************************************
*  BaseVertex
*  <TODO: insert class description here>
*
*
*  @remarks <TODO: insert remarks here>
*
*  @author Yan Qi @date 6/6/2010
**************************************************************************/
class BaseVertex
{
    int m_nID;
    double m_dWeight;

public:
    int getID() const { return m_nID; }
    void setID(int ID_) { m_nID = ID_; }

    double Weight() const { return m_dWeight; }
    void Weight(double val) { m_dWeight = val; }

    void PrintOut(std::ostream& out_stream)
    {
        out_stream << decToStr(m_nID);
    }

    // return string
    string stringOut() {
        return decToStr(m_nID);
    }

    string decToStr(int nodeID) {
        string result;
        int letter = nodeID / 10;
        int number = nodeID % 10;
        char let = static_cast<char>(letter+48);
        result = let + to_string(number);
        return result;
    }

};


/**************************************************************************
*  BasePath
*  <TODO: insert class description here>
*
*
*  @remarks <TODO: insert remarks here>
*
*  @author Yan Qi @date 6/6/2010
**************************************************************************/
class BasePath
{
protected:

    int m_nLength;
    double m_dWeight;
    std::vector<BaseVertex*> m_vtVertexList;

public:
    BasePath(const std::vector<BaseVertex*>& vertex_list, double weight)
        :m_dWeight(weight)
    {
        m_vtVertexList.assign(vertex_list.begin(), vertex_list.end());
        m_nLength = m_vtVertexList.size();
    }
    ~BasePath(void){}

    double Weight() const { return m_dWeight; }
    void Weight(double val) { m_dWeight = val; }

    int length() { return m_nLength; }

    BaseVertex* GetVertex(int i)
    {
        return m_vtVertexList.at(i);
    }

    bool SubPath(std::vector<BaseVertex*>& sub_path, BaseVertex* ending_vertex_pt)
    {

        for (std::vector<BaseVertex*>::const_iterator pos = m_vtVertexList.begin();
            pos != m_vtVertexList.end(); ++pos)
        {
            if (*pos != ending_vertex_pt)
            {
                sub_path.push_back(*pos);
            }else
            {
                //break;
                return true;
            }
        }

        return false;
    }

    // display the content
    void PrintOut(std::ostream& out_stream) const
    {
        out_stream << "Cost: " << m_dWeight << " Length: " << m_vtVertexList.size() << std::endl;
        for(std::vector<BaseVertex*>::const_iterator pos=m_vtVertexList.begin(); pos!=m_vtVertexList.end();++pos)
        {
            (*pos)->PrintOut(out_stream);
            out_stream << "->";
        }
        out_stream << std::endl <<  "*********************************************" << std::endl;
    }

    // return string
    string stringOut() const
    {
        string result = "";
        for(std::vector<BaseVertex*>::const_iterator pos=m_vtVertexList.begin(); pos!=m_vtVertexList.end();++pos)
        {
            result += (*pos)->stringOut();
            result += "->";
        }
        return result;
    }

    string edgelistOut() const
    {
        string result = "";
        string temp = "";
        bool split = false;
        int count = 0;
        for(std::vector<BaseVertex*>::const_iterator pos=m_vtVertexList.begin(); pos!=m_vtVertexList.end();++pos)
        {
            if(count % 2 == 0 && count != 0) {
                result += " " + temp;
                split = true;
            }
            temp = (*pos)->stringOut();
            result += temp;
            count++;
            // if the count equals to length we reached the last node
            if(split && m_nLength != count) {
                result += " " + temp;
                split = false;
            }
        }
        return result;
    }
};
