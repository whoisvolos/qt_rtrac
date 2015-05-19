#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <qmessagebox.h>
#include <QtCore/qcoreapplication.h>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_xRot(0), m_yRot(0), m_zRot(0), m_scale(1.0f), m_program(0), m_model(NULL), m_core(false) {
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TranslucentBackground);

    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));

    setAutoFillBackground(false);
    connect(this, SIGNAL(modelLoading(const char*)), this, SLOT(loadModel(const char*)));
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

void GLWidget::loadModel(const char *filePath) {
    auto new_model = ObjLoader::loadModel(filePath);
    if (!new_model) {
        QMessageBox::warning(this, "Loading error", "Can not load this model");
    } else {
        delete m_model;
        m_model = new_model;

        makeCurrent();
        m_vbo.destroy();
        m_vao.destroy();
        delete m_program;
        m_program = 0;
        initializeProgram();
        doneCurrent();

        update();
    }
}

void GLWidget::initializeProgram() {
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, m_core ? "shaders/vertex-core.glsl" : "shaders/vertex.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, m_core ? "shaders/fragment-core.glsl" : "shaders/fragment.glsl");
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    m_vao.create();
    m_vao.bind();
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(m_model->constData(), m_model->count() * sizeof(GLfloat));
    setupVertexAttribs();

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -2);

    m_program->setUniformValue(m_lightPosLoc, QVector3D(10, 0, 50));

    m_program->release();
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0, 0);

    //initializeProgram();
}

void GLWidget::setupVertexAttribs() {
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void GLWidget::paintGL() {
    if (m_model) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        m_world.setToIdentity();
        m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
        m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
        m_world.rotate(m_zRot / 16.0f, 0, 0, 1);
        m_world.scale(m_scale);

        m_vao.bind();
        m_program->bind();
        m_program->setUniformValue(m_projMatrixLoc, m_proj);
        m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
        QMatrix3x3 normalMatrix = m_world.normalMatrix();
        m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

        glDrawArrays(GL_TRIANGLES, 0, m_model->vertexCount());

        m_program->release();
    }
}

void GLWidget::resizeGL(int w, int h) {
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    m_lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    m_scale += ((float)(event->delta() / 120)) / 8;
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}