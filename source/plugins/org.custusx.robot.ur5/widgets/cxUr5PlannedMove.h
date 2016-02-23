#ifndef UR5PLANNEDMOVE_H
#define UR5PLANNEDMOVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTextEdit>


namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5PlannedMoveTab : public QWidget
{
    Q_OBJECT
public:
    Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5PlannedMoveTab();

private slots:
    void runVTKfileSlot();
    void openVTKfileSlot();
    void blendRadiusChangedSlot();
    void runVelocityVTKSlot();

    void clearPoseQueueSlot();

    void moveToPointSlot();
    void moveToFrameSlot();
    void followActiveToolSlot();
    void startFollowingActiveToolSlot(Transform3D matrix, double timestamp);

    void logActiveToolSlot();
    void startLogActiveToolSlot(Transform3D matrix, double timestamp);
    void runLoggedActiveToolSlot();

private:
    QPushButton *runVTKButton, *openVTKButton, *runVelocityVTKButton;

    QPushButton *clearPoseQueueButton;
    QPushButton *moveToInitialPositionButton;

    QPushButton *moveToPointButton, *moveToFrameButton, *followActiveToolButton;
    QPushButton *logActiveToolButton, *runLoggedActiveToolButton;

    QLineEdit *vtkLineEdit;
    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit, *blendRadiusLineEdit;


    void setupUi(QWidget *parent);

    void setMoveVTKWidget(QVBoxLayout *parent);
    void setMoveSettingsWidget(QVBoxLayout *parent);
    void setMovementAssignmentMetricWidget(QVBoxLayout *parent);
    void setMovementAssignmentToolWidget(QVBoxLayout *parent);

    Ur5ProgramEncoder mProgramEncoder;

    VisServicesPtr mServices;
    Ur5RobotPtr mUr5Robot;

    QString getPathToPlugin();

    MovementQueue mMovementQueue;
    std::vector<Transform3D> mTransforms;
};

} // cx

#endif // UR5PLANNEDMOVE_H
