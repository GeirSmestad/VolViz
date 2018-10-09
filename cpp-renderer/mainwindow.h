#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidgetslicer.h"
#include "glwidgetdvr.h"
#include "glwidgetcube.h"
#include "Volume.h"

// #include "gl_tf_editor.h"

class QAction;
class QMenu;
class QLabel;
class QMenuBar;
class QTextEdit;
class QDoubleSpinBox;
class QWidget;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QSplitter;
class GLWidget;
class GLWidgetDvr;
class Volume;
class QSlider;
class QPushButton;
class QSpacerItem;
class QComboBox;
class QCheckBox;
class WindowingDialog;

/// Main window of the application
class MainWindow : public QMainWindow
{
    Q_OBJECT

    // ************************************************************************************************************
    // *** Basic methods ******************************************************************************************
public:
    /// Default constructor
    MainWindow(QWidget *parent = 0) : QMainWindow(parent)
    {
        // Set the window size
        this->resize(800,600);
        this->setWindowTitle(QApplication::translate("MainWindowClass", "MainWindow", 0, QApplication::UnicodeUTF8));

        setupUi();
    }

    /// Destructor
    ~MainWindow()
    {
        // Deallocate everything
        delete m_actionLoadDataset;

        delete m_tabWidget;
        delete m_tabSlicer;
        delete m_tabCube;
        delete m_tabVolumeRendering;
        delete m_widgetSlicerControl;
        delete m_widgetDvrControl;

        delete m_label1_Slicer;
        delete m_label1_Dvr;
        delete m_label2_Dvr;
        delete m_label3_Dvr;
        delete m_label4_Dvr;
        delete m_label5_Dvr;
        delete m_label6_Dvr;
        delete m_label7_Dvr;
        delete m_label8_Dvr;

        delete m_layoutSlicer;
        delete m_layoutSlicerControl;
        delete m_layoutDvr;
        delete m_layoutDvrControl;
        delete m_layoutCube;

        delete m_splitterSlicer;
        delete m_splitterDvr;

        delete m_combo_slicerAxis;
        delete m_combo_dvrInterpolationMethod;
        delete m_combo_dvrGradientInterpolationMethod;
        delete m_combo_dvrGradientMethod;
        delete m_combo_dvrProjection;
        delete m_combo_dvrShading;
        delete m_combo_dvrRenderingMethod;
        delete m_combo_dvrRes;
        delete m_combo_dvrResRotating;
        delete m_combo_dvrTfMode;

        delete m_spacer1_Dvr;
        delete m_spacer2_Dvr;
        delete m_spacer1_Slicer;

        delete m_spinBox_dvrStepSize;
        delete m_hSlider_Slicer;
        delete m_hSlider_DvrFhit;

        delete m_check_dvrAdaptiveStep;
        delete m_check_dvrPrecomputeGradients;
        delete m_check_slicerFree;

        delete m_push_dvrTf;
        delete m_push_slicerTf;

        delete m_menubar;
        delete m_menuFile;

        delete m_glwidgetSlicer;
        delete m_glwidgetDvr;
        delete m_glwidgetCube;
    } /* destructor */

    // ************************************************************************************************************
    // *** Slots **************************************************************************************************
public slots:
    /// Load a dataset
    void open() {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Dataset"),
            "",
            tr("DataSet (*.dat);;All Files (*)"));
        if (!fileName.isEmpty()) {
            m_volume.loadVolumeDat(fileName.toStdString());
            emit newVolume(&m_volume);
        }
        resetDvrTab();
    }

    /// Set the maximum value of the slicer slider
    void setMaxSliceNumber(int max) {
        std::cout << "Debug: Set max slice number." << std::endl;

        m_hSlider_Slicer->setRange(0, max);
        m_hSlider_Slicer->setValue(0);
    }


    /*
    void openWindowingDialog() {
        std::cout << "Debug: creating transfer function widget" << std::endl;
        m_trFunctEditor = new GLTrFunctEditor();
        m_trFunctEditor->setObjectName(QString::fromUtf8("transferFunction"));
        m_trFunctEditor->setWindowTitle(QApplication::translate("transferFunction", "Edit transfer function"));

        m_trFunctEditor->show();
    }
    */

    // ************************************************************************************************************
    // *** Signals ************************************************************************************************
signals:
    /// Notify that new volume data has been loaded
    void newVolume(Volume *v);


    // ************************************************************************************************************
    // *** Private methods ****************************************************************************************
private:
    /// Setup the graphical user interface
    void setupUi() {
        // Create the tab pane
        m_tabWidget = new QTabWidget;
        m_tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        m_tabWidget->setTabPosition(QTabWidget::West);
        m_tabWidget->setTabShape(QTabWidget::Triangular);
        setCentralWidget(m_tabWidget);

        // Create the tabs in it
        createMenus();
        createCubeTab();
        createSlicerTab();
        createDvrTab();

        // Setup the connections
        slicerConnections();

        std::cout << "Debug: Setting up slicer connections." << std::endl;

        dvrConnections();

        // Set the widgets initial status
        resetDvrTab();

        // Set widgets labels
        m_menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0, QApplication::UnicodeUTF8));

        m_check_slicerFree->setText(QApplication::translate("MainWindowClass", "Free Axis", 0, QApplication::UnicodeUTF8));
        m_push_slicerTf->setText(QApplication::translate("MainWindowClass", "Levels editor", 0, QApplication::UnicodeUTF8));
        m_push_dvrTf->setText(QApplication::translate("MainWindowClass", "TransferFunction editor", 0, QApplication::UnicodeUTF8));

        m_tabWidget->setTabText(m_tabWidget->indexOf(m_tabSlicer), QApplication::translate("MainWindowClass", "Slicer", 0, QApplication::UnicodeUTF8));
        m_tabWidget->setTabText(m_tabWidget->indexOf(m_tabVolumeRendering), QApplication::translate("MainWindowClass", "Volume Rendering", 0, QApplication::UnicodeUTF8));
        m_tabWidget->setTabText(m_tabWidget->indexOf(m_tabCube), QApplication::translate("MainWindowClass", "Cube", 0, QApplication::UnicodeUTF8));
    } /* setupUi() */

    /// Set the connections for the slicer tab
    void slicerConnections() {
        //  LoadData()

        connect(this, SIGNAL(newVolume(Volume *)), m_glwidgetSlicer, SLOT(setVolume(Volume *)));

        //connect(m_push_slicerTf, SIGNAL(clicked()),this, SLOT(openWindowingDialog()));
        connect(m_hSlider_Slicer, SIGNAL(valueChanged(int)), m_glwidgetSlicer, SLOT(setSlice(int)));
        connect(m_glwidgetSlicer, SIGNAL(maxSlicesNumberChanged(int)), this, SLOT(setMaxSliceNumber(int)));
        connect(m_combo_slicerAxis, SIGNAL(activated(int)), m_glwidgetSlicer, SLOT(changeAxis(int)));
    }

    /// Set the connections for the dvr tab
    void dvrConnections() {
        //  LoadData()
        connect(this, SIGNAL(newVolume(Volume *)), m_glwidgetDvr, SLOT(setVolume(Volume *)));

        connect(m_combo_dvrRes, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setRes(int)));
        connect(m_combo_dvrResRotating, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setResRotating(int)));
        connect(m_combo_dvrInterpolationMethod, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setInterpolationMode(int)));
        connect(m_combo_dvrGradientInterpolationMethod, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setGradientInterpolationMode(int)));
        //connect(m_combo_dvrGradientMethod, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setGradientMode(int)));
        connect(m_combo_dvrProjection, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setViewingMode(int)));
        connect(m_combo_dvrShading, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setShading(int)));
        connect(m_combo_dvrTfMode, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setTfMode(int)));
        connect(m_combo_dvrRenderingMethod, SIGNAL(activated(int)), m_glwidgetDvr, SLOT(setRenderingMode(int)));
        connect(m_spinBox_dvrStepSize, SIGNAL(valueChanged(double)), m_glwidgetDvr, SLOT(setStepSize(double)));
        connect(m_hSlider_DvrFhit, SIGNAL(valueChanged(int)), m_glwidgetDvr, SLOT(setHitValue(int)));
        //connect(m_push_dvrTf, SIGNAL(clicked()),this, SLOT(openWindowingDialog()));
        connect(m_push_dvrTf, SIGNAL(clicked()), m_glwidgetDvr, SLOT(openWindowingDialog()));
    }

    /// Add widgets to the cube tab
    void createCubeTab() {
        m_tabCube = new QWidget();
        m_tabCube->setObjectName(QString::fromUtf8("tabCube"));
        m_layoutCube = new QHBoxLayout(m_tabCube);
        m_layoutCube->setObjectName(QString::fromUtf8("layoutCube"));

        m_glwidgetCube = new GLWidgetCube(m_tabCube);
        m_layoutCube->addWidget(m_glwidgetCube);
        m_tabWidget->addTab(m_tabCube, QString());
    }

    /// Add widgets to the slicer tab
	void createSlicerTab() {
        // --- Create the slicer tab ---
		m_tabSlicer = new QWidget();
		m_tabSlicer->setObjectName(QString::fromUtf8("tabSlicer"));
        m_tabWidget->addTab(m_tabSlicer, QString()); // Add this tab to the tab pane

        // Layout
		m_layoutSlicer = new QHBoxLayout(m_tabSlicer);
		m_layoutSlicer->setObjectName(QString::fromUtf8("layoutSlicer"));

        // Split the tab in two using a splitter
		m_splitterSlicer = new QSplitter(m_tabSlicer);
		m_splitterSlicer->setObjectName(QString::fromUtf8("splitterSlicer"));
		m_splitterSlicer->setOrientation(Qt::Horizontal);
        m_layoutSlicer->addWidget(m_splitterSlicer); // Add the splitter to the tab


        // --- Setup a control panel on the left side of the slicer tab ---
		m_widgetSlicerControl = new QWidget(m_splitterSlicer);
		m_widgetSlicerControl->setObjectName(QString::fromUtf8("widgetSlicerControl"));
        m_widgetSlicerControl->setMaximumWidth(200);
        m_splitterSlicer->addWidget(m_widgetSlicerControl);

        // layout
		m_layoutSlicerControl = new QVBoxLayout(m_widgetSlicerControl);
		m_layoutSlicerControl->setObjectName(QString::fromUtf8("layoutSlicerControl"));

		// first label
		m_label1_Slicer = new QLabel(m_widgetSlicerControl);
		m_label1_Slicer->setObjectName(QString::fromUtf8("label1_Slicer"));
		m_label1_Slicer->setText(QApplication::translate("MainWindowClass", "Slicing through axis", 0, QApplication::UnicodeUTF8));
		m_layoutSlicerControl->addWidget(m_label1_Slicer);

        // the combobox
		m_combo_slicerAxis = new QComboBox(m_widgetSlicerControl);
		m_combo_slicerAxis->setObjectName(QString::fromUtf8("combo_slicerAxis"));
		m_combo_slicerAxis->addItem(tr("z"));
		m_combo_slicerAxis->addItem(tr("x"));
		m_combo_slicerAxis->addItem(tr("y"));
		m_layoutSlicerControl->addWidget(m_combo_slicerAxis);

		// the checkbox
		m_check_slicerFree = new QCheckBox(m_widgetSlicerControl);
		m_check_slicerFree->setObjectName(QString::fromUtf8("check_slicerFree"));
        m_check_slicerFree->setText(QApplication::translate("MainWindowClass", "Free Axis", 0, QApplication::UnicodeUTF8));
		m_layoutSlicerControl->addWidget(m_check_slicerFree);

		// the vspacer
		m_spacer1_Slicer = new QSpacerItem(20, 480, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_layoutSlicerControl->addItem(m_spacer1_Slicer);

		// the slider
		m_hSlider_Slicer = new QSlider(m_widgetSlicerControl);
		m_hSlider_Slicer->setObjectName(QString::fromUtf8("hSlider_Slicer"));
		m_hSlider_Slicer->setOrientation(Qt::Horizontal);
		m_hSlider_Slicer->setRange(0,0);
		m_layoutSlicerControl->addWidget(m_hSlider_Slicer);

		// the pushbutton
		m_push_slicerTf = new QPushButton(m_widgetSlicerControl);
		m_push_slicerTf->setObjectName(QString::fromUtf8("push_slicerTf"));
        m_push_slicerTf->setText(QApplication::translate("MainWindowClass", "Levels editor", 0, QApplication::UnicodeUTF8));
		m_layoutSlicerControl->addWidget(m_push_slicerTf);


        // --- Setup an OpenGL widget on the right side of the slicer tab ---
        m_glwidgetSlicer = new GLWidgetSlicer(m_splitterSlicer);
		m_splitterSlicer->addWidget(m_glwidgetSlicer);
    } /* createSlicerTab() */


    /// Add widgets to the dvr tab
	void createDvrTab() {
        // --- Create the dvr tab ---
		m_tabVolumeRendering = new QWidget();
		m_tabVolumeRendering->setObjectName(QString::fromUtf8("tabVolumeRendering"));
        m_tabWidget->addTab(m_tabVolumeRendering, QString());

        // Layout
		m_layoutDvr = new QHBoxLayout(m_tabVolumeRendering);
		m_layoutDvr->setObjectName(QString::fromUtf8("layoutDvr"));

        // Split the tab in two using a splitter
        m_splitterDvr = new QSplitter(m_tabVolumeRendering);
		m_splitterDvr->setObjectName(QString::fromUtf8("splitterDvr"));
		m_splitterDvr->setOrientation(Qt::Horizontal);
        m_layoutDvr->addWidget(m_splitterDvr);


        // --- Setup a control panel on the left side of the dvr tab ---
		m_widgetDvrControl = new QWidget(m_splitterDvr);
		m_widgetDvrControl->setObjectName(QString::fromUtf8("widgetDvrControl"));
        m_widgetDvrControl->setMaximumWidth(200);
        m_splitterDvr->addWidget(m_widgetDvrControl);

        // layout
		m_layoutDvrControl = new QVBoxLayout(m_widgetDvrControl);
		m_layoutDvrControl->setObjectName(QString::fromUtf8("layoutDvrControl"));

        // all the other widgets
        m_label1_Dvr = new QLabel(m_widgetDvrControl);
		m_label1_Dvr->setObjectName(QString::fromUtf8("label1_Dvr"));
		m_label1_Dvr->setText(QApplication::translate("MainWindowClass", "Interpolation method", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label1_Dvr);

		m_combo_dvrInterpolationMethod = new QComboBox(m_widgetDvrControl);
		m_combo_dvrInterpolationMethod->setObjectName(QString::fromUtf8("combo_dvrInterpolationMethod"));
		m_combo_dvrInterpolationMethod->addItem(tr("Nearest"));
		m_combo_dvrInterpolationMethod->addItem(tr("Trilinear"));
		m_layoutDvrControl->addWidget(m_combo_dvrInterpolationMethod);

		m_combo_dvrGradientInterpolationMethod = new QComboBox(m_widgetDvrControl);
		m_combo_dvrGradientInterpolationMethod->setObjectName(QString::fromUtf8("combo_dvrGradientInterpolationMethod"));
		m_combo_dvrGradientInterpolationMethod->addItem(tr("Nearest"));
		m_combo_dvrGradientInterpolationMethod->addItem(tr("Trilinear"));
		m_layoutDvrControl->addWidget(m_combo_dvrGradientInterpolationMethod);

		m_label2_Dvr = new QLabel(m_widgetDvrControl);
		m_label2_Dvr->setObjectName(QString::fromUtf8("label2_Dvr"));
		m_label2_Dvr->setText(QApplication::translate("MainWindowClass", "Gradient Computation Method", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label2_Dvr);

		m_combo_dvrGradientMethod = new QComboBox(m_widgetDvrControl);
		m_combo_dvrGradientMethod->setObjectName(QString::fromUtf8("combo_dvrGradientMethod"));
		m_combo_dvrGradientMethod->addItem(tr("Central Diff."));
        //m_combo_dvrGradientMethod->addItem(tr("Forward Diff."));
        //m_combo_dvrGradientMethod->addItem(tr("Backward Diff."));
		m_layoutDvrControl->addWidget(m_combo_dvrGradientMethod);

		m_label3_Dvr = new QLabel(m_widgetDvrControl);
		m_label3_Dvr->setObjectName(QString::fromUtf8("label3_Dvr"));
		m_label3_Dvr->setText(QApplication::translate("MainWindowClass", "Viewing projection", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label3_Dvr);

		m_combo_dvrProjection = new QComboBox(m_widgetDvrControl);
		m_combo_dvrProjection->setObjectName(QString::fromUtf8("combo_dvrProjection"));
		m_combo_dvrProjection->addItem(tr("Parallel"));
        m_combo_dvrProjection->addItem(tr("Perspective"));
		m_layoutDvrControl->addWidget(m_combo_dvrProjection);

		m_label4_Dvr = new QLabel(m_widgetDvrControl);
		m_label4_Dvr->setObjectName(QString::fromUtf8("label4_Dvr"));
		m_label4_Dvr->setText(QApplication::translate("MainWindowClass", "Shading", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label4_Dvr);

		m_combo_dvrShading = new QComboBox(m_widgetDvrControl);
		m_combo_dvrShading->setObjectName(QString::fromUtf8("combo_dvrShading"));
		m_combo_dvrShading->addItem(tr("None"));
		m_combo_dvrShading->addItem(tr("Phong"));
		m_layoutDvrControl->addWidget(m_combo_dvrShading);

		m_label5_Dvr = new QLabel(m_widgetDvrControl);
		m_label5_Dvr->setObjectName(QString::fromUtf8("label5_Dvr"));
		m_label5_Dvr->setText(QApplication::translate("MainWindowClass", "Rendering method", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label5_Dvr);

		m_combo_dvrRenderingMethod = new QComboBox(m_widgetDvrControl);
		m_combo_dvrRenderingMethod->setObjectName(QString::fromUtf8("combo_dvrRenderingMethod"));
        m_combo_dvrRenderingMethod->addItem(tr("First Hit"));
        m_combo_dvrRenderingMethod->addItem(tr("Maximum Intensity"));
        m_combo_dvrRenderingMethod->addItem(tr("Average"));
        m_combo_dvrRenderingMethod->addItem(tr("DVR"));
		m_layoutDvrControl->addWidget(m_combo_dvrRenderingMethod);

		m_label7_Dvr = new QLabel(m_widgetDvrControl);
		m_label7_Dvr->setObjectName(QString::fromUtf8("label7_Dvr"));
		m_label7_Dvr->setText(QApplication::translate("MainWindowClass", "Rendering resolution", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label7_Dvr);
		m_combo_dvrRes = new QComboBox(m_widgetDvrControl);
		m_combo_dvrRes->setObjectName(QString::fromUtf8("combo_dvrRes"));
		m_combo_dvrRes->addItem(tr("64x64"));
		m_combo_dvrRes->addItem(tr("128x128"));
		m_combo_dvrRes->addItem(tr("256x256"));
		m_combo_dvrRes->addItem(tr("512x512"));
		m_combo_dvrRes->addItem(tr("1024x1024"));
		m_layoutDvrControl->addWidget(m_combo_dvrRes);

		m_label8_Dvr = new QLabel(m_widgetDvrControl);
		m_label8_Dvr->setObjectName(QString::fromUtf8("label8_Dvr"));
		m_label8_Dvr->setText(QApplication::translate("MainWindowClass", "Moving resolution", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label8_Dvr);

		m_combo_dvrResRotating = new QComboBox(m_widgetDvrControl);
		m_combo_dvrResRotating->setObjectName(QString::fromUtf8("combo_dvrResRotating"));
		m_combo_dvrResRotating->addItem(tr("16x16"));
		m_combo_dvrResRotating->addItem(tr("32x32"));
		m_combo_dvrResRotating->addItem(tr("64x64"));
		m_combo_dvrResRotating->addItem(tr("128x128"));
        m_combo_dvrResRotating->setCurrentIndex(2);
		m_layoutDvrControl->addWidget(m_combo_dvrResRotating);

		m_combo_dvrTfMode = new QComboBox(m_widgetDvrControl);
		m_combo_dvrTfMode->setObjectName(QString::fromUtf8("combo_dvrTfMode"));
		m_combo_dvrTfMode->addItem(tr("1D Transfer Function"));
        //m_combo_dvrTfMode->addItem(tr("1D Logarithmic Transfer Function"));
        m_combo_dvrTfMode->addItem(tr("1D, Gradient-based transparency"));
        //m_combo_dvrTfMode->addItem(tr("2D Logarithmic Transfer Function"));
		m_layoutDvrControl->addWidget(m_combo_dvrTfMode);

		m_label6_Dvr = new QLabel(m_widgetDvrControl);
		m_label6_Dvr->setObjectName(QString::fromUtf8("label6_Dvr"));
		m_label6_Dvr->setText(QApplication::translate("MainWindowClass", "Step size", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_label6_Dvr);

		m_spinBox_dvrStepSize = new QDoubleSpinBox(m_widgetDvrControl);
		m_spinBox_dvrStepSize->setObjectName(QString::fromUtf8("spinBox_dvrStepSize"));
        m_spinBox_dvrStepSize->setRange(0.001,0.02);
        m_spinBox_dvrStepSize->setSingleStep(0.001);
        m_spinBox_dvrStepSize->setDecimals(3);
		m_layoutDvrControl->addWidget(m_spinBox_dvrStepSize);


        //check_dvrAdaptiveStep = new QCheckBox(widgetDvrControl);
        //check_dvrAdaptiveStep->setObjectName(QString::fromUtf8("check_dvrAdaptiveStep"));
        //check_dvrAdaptiveStep->setText(QApplication::translate("MainWindowClass", "Adaptive step size", 0, QApplication::UnicodeUTF8));
        //layoutDvrControl->addWidget(check_dvrAdaptiveStep);

		m_hSlider_DvrFhit = new QSlider(m_widgetDvrControl);
		m_hSlider_DvrFhit->setObjectName(QString::fromUtf8("hSlider_DvrFhit"));
		m_hSlider_DvrFhit->setOrientation(Qt::Horizontal);
        m_hSlider_DvrFhit->setRange(0,100);
		m_layoutDvrControl->addWidget(m_hSlider_DvrFhit);

		m_spacer2_Dvr = new QSpacerItem(20, 110, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_layoutDvrControl->addItem(m_spacer2_Dvr);

		m_push_dvrTf = new QPushButton(m_widgetDvrControl);
		m_push_dvrTf->setObjectName(QString::fromUtf8("push_dvrTf"));
        m_push_dvrTf->setText(QApplication::translate("MainWindowClass", "TransferFunction editor", 0, QApplication::UnicodeUTF8));
		m_layoutDvrControl->addWidget(m_push_dvrTf);

        // --- Setup an OpenGL widget on the right side of the slicer tab ---
        m_glwidgetDvr = new GLWidgetDvr(m_splitterDvr);
        m_glwidgetDvr->setName("GLWidgetDvr");

        m_splitterDvr->addWidget(m_glwidgetDvr);
    } /* createDvrTab() */

    /// Reset the dvr options to the default settings
	void resetDvrTab() {
		m_combo_dvrRes->setCurrentIndex(0);
		m_combo_dvrResRotating->setCurrentIndex(0);
		m_combo_dvrInterpolationMethod->setCurrentIndex(0);
		m_combo_dvrGradientMethod->setCurrentIndex(0);
		m_combo_dvrGradientInterpolationMethod->setCurrentIndex(0);
		m_combo_dvrProjection->setCurrentIndex(0);
		m_combo_dvrShading->setCurrentIndex(0);
		m_combo_dvrRenderingMethod->setCurrentIndex(0);
		m_combo_dvrTfMode->setCurrentIndex(0);
		m_hSlider_DvrFhit->setValue(0);
        m_spinBox_dvrStepSize->setValue(0.1);
	}

    /// Create the menus for the main window
    void createMenus() {
		m_actionLoadDataset = new QAction( tr("&Load dataset"),this);
		m_actionLoadDataset->setObjectName(QString::fromUtf8("actionLoad_Dataset"));
		m_actionLoadDataset->setStatusTip(tr("Open Dataset"));
		connect(m_actionLoadDataset, SIGNAL(triggered()), this, SLOT(open()));

        std::cout << "Debug: Connected main window menus." << std::endl << std::endl;

        m_menubar = new QMenuBar(this);
		m_menubar->setObjectName(QString::fromUtf8("menubar"));
		m_menubar->setGeometry(QRect(0, 0, 1031, 22));
        this->setMenuBar(m_menubar);

		m_menuFile = new QMenu(m_menubar);
		m_menuFile->setObjectName(QString::fromUtf8("menuFile"));
        m_menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0, QApplication::UnicodeUTF8));
        m_menuFile->addAction(m_actionLoadDataset);
        m_menubar->addAction(m_menuFile->menuAction());
    } /* createMenus() */


    // ************************************************************************************************************
    // *** Class members ******************************************************************************************
private:
    Volume m_volume;

    GLWidgetCube *m_glwidgetCube;
    GLWidgetSlicer *m_glwidgetSlicer;
    GLWidgetDvr *m_glwidgetDvr;

    QAction *m_actionLoadDataset;

    QTabWidget *m_tabWidget;
    QWidget *m_tabSlicer;
	QWidget *m_tabCube;
    QWidget *m_tabVolumeRendering;
    QWidget *m_widgetSlicerControl;
    QWidget *m_widgetDvrControl;

    QLabel *m_label1_Slicer;
    QLabel *m_label1_Dvr;
    QLabel *m_label2_Dvr;
    QLabel *m_label3_Dvr;
    QLabel *m_label4_Dvr;
    QLabel *m_label5_Dvr;
    QLabel *m_label6_Dvr;
    QLabel *m_label7_Dvr;
    QLabel *m_label8_Dvr;

    QHBoxLayout *m_layoutSlicer;
    QVBoxLayout *m_layoutSlicerControl;
    QHBoxLayout *m_layoutDvr;
    QVBoxLayout *m_layoutDvrControl;
    QHBoxLayout *m_layoutCube;

    QSplitter *m_splitterSlicer;
    QSplitter *m_splitterDvr;

    QComboBox *m_combo_slicerAxis;
    QComboBox *m_combo_dvrInterpolationMethod;
    QComboBox *m_combo_dvrGradientInterpolationMethod;
    QComboBox *m_combo_dvrGradientMethod;
    QComboBox *m_combo_dvrProjection;
    QComboBox *m_combo_dvrShading;
    QComboBox *m_combo_dvrRenderingMethod;
    QComboBox *m_combo_dvrRes;
    QComboBox *m_combo_dvrResRotating;
    QComboBox *m_combo_dvrTfMode;

    QSpacerItem *m_spacer1_Dvr;
    QSpacerItem *m_spacer2_Dvr;
    QSpacerItem *m_spacer1_Slicer;

    QDoubleSpinBox *m_spinBox_dvrStepSize;
    QSlider *m_hSlider_Slicer;
    QSlider *m_hSlider_DvrFhit;

    QCheckBox *m_check_dvrAdaptiveStep;
    QCheckBox *m_check_dvrPrecomputeGradients;
    QCheckBox *m_check_slicerFree;

    QPushButton *m_push_dvrTf;
    QPushButton *m_push_slicerTf;

    QMenuBar *m_menubar;
    QMenu *m_menuFile;


/*
    GLTrFunctEditor* m_trFunctEditor;
    TransferFunction* m_transferFunction; // TODO: Implement this
    */
}; /* class MainWindow */

#endif // MAINWINDOW_H


