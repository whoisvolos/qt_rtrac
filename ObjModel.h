
#ifndef QT_RTRAC_OBJMODEL_H
#define QT_RTRAC_OBJMODEL_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>

class ObjModel {
public:
    ObjModel(QVector<QVector3D> *data) : all_data(data) { }

    ~ObjModel() {
        delete all_data;
    }

    const GLfloat * constData() const {
        return (GLfloat const *) all_data->constData();
    }

    const int count() {
        return all_data->size() * 3;
    }

    const int vertexCount() {
        return count() / 2;
    }

private:
    QVector<QVector3D> *all_data;
};


#endif //QT_RTRAC_OBJMODEL_H
