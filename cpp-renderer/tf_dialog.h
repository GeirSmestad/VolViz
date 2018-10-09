#ifndef TF_DIALOG_H
#define TF_DIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include "gl_tf_editor.h"
#include "transfer_function.h"


/// Dialog to edit a transfer function
class TFDialog : public QDialog {
    Q_OBJECT

    // ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Constructor
    TFDialog(QWidget *parent = 0) : QDialog(parent), m_trFunct(NULL) {
        // OpenGL editor widget
        m_trFunctEditor = new GLTrFunctEditor(this);

        // Create buttons
        m_btOpen = new QPushButton(tr("&Open"));
        m_btOpen->setDefault(true);
        m_btSave = new QPushButton(tr("&Save"));
        m_btReset = new QPushButton(tr("&Reset"));

        // Button box
        m_bxButtons = new QDialogButtonBox(Qt::Horizontal);
        m_bxButtons->addButton(m_btOpen, QDialogButtonBox::AcceptRole);
        m_bxButtons->addButton(m_btSave, QDialogButtonBox::ActionRole);
        m_bxButtons->addButton(m_btReset,QDialogButtonBox::ActionRole);

        // Checkbox
        // m_ckLogarithmic = new QCheckBox(tr("&Logaritmic Scale")); // Don't want this for now.

        // Layout
        m_layout = new QVBoxLayout;
        // m_layout->addWidget(m_ckLogarithmic);
        m_layout->addWidget(m_trFunctEditor);
        m_layout->addWidget(m_bxButtons);

        // Connections and dialog setup
        connectWidgets(parent);
        setLayout(m_layout);
        setWindowTitle(tr("Transfer function editor"));
        this->resize(450, 220);

        m_trFunct = new TransferFunction();
        m_trFunctEditor->setTransferFunction(*m_trFunct);


        // TODO: Should read in dataset and paint histogram in the background of the transfer function editor.
    } /* Constructor */


    /// Destructor
    virtual ~TFDialog() {
        delete m_trFunctEditor;
        delete m_btReset;
        delete m_btOpen;
        delete m_btSave;
        delete m_bxButtons;
        // delete m_ckLogarithmic;
        delete m_layout;
        delete m_trFunct;
    }

    // ********************************************************************************************************
    // *** Public slots ***************************************************************************************
public slots:
    /// Reset the transfer function
    void tfReset() {
        if(m_trFunct) {
            m_trFunct->Reset();
            m_trFunctEditor->updateGL();
        }
     }

    /// Load a trasnfer function
    void tfOpen() {
        if(m_trFunct) {
            QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Open Transfer Function"),
                    "",
                    tr("Transfer Function (*.tff);;All Files (*)"));
            if (!fileName.isEmpty()) {
                m_trFunct->open(fileName.toStdString());
            }
        }
    }

    /// Store a transfer function
    void tfSave() {
        if(m_trFunct) {
            QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Save transfer function"),
                    "",
                    tr("Transfer Function (*.tff);;All Files (*)"));
            if (!fileName.isEmpty()) {
                m_trFunct->save(fileName.toStdString());
            }
        }
     }


    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Set the transfer function to edit
    void setTransferFunction(TransferFunction& tf) {
        m_trFunctEditor->setTransferFunction(tf);
        m_trFunct = &tf;
    }

    TransferFunction* GetTransferFunction() {
        return m_trFunct;
    }

    void SetHistogram(vector<float> parameterHistogram) {
        std::cout << "Histogram was set." << std::endl;

        m_trFunctEditor->setHistogram(parameterHistogram);
    }

    // ********************************************************************************************************
    // *** Class members **************************************************************************************
private:
    TransferFunction * m_trFunct; ///< reference to the transfer function being edited
    GLTrFunctEditor * m_trFunctEditor; ///< OpenGL widget for transfer function editing

    QPushButton *m_btOpen;
    QPushButton *m_btSave;
    QPushButton *m_btReset;
    // QCheckBox *m_ckLogarithmic;
    QDialogButtonBox *m_bxButtons;
    QVBoxLayout *m_layout;


    // ********************************************************************************************************
    // *** Private methods ************************************************************************************
    void connectWidgets(QWidget *parent = 0) {
        connect(m_btReset, SIGNAL(clicked()),this, SLOT(tfReset()));
        connect(m_btOpen, SIGNAL(clicked()),this, SLOT(tfOpen()));
        connect(m_btSave, SIGNAL(clicked()),this, SLOT(tfSave()));

        // Enable this connection to an appropriate slot once you have implemented the support
        // for logarithmic transfer functions
        //connect(m_ckLogarithmic, SIGNAL(stateChanged(int)), /*TODO*/, SLOT( /*TODO*/ ));
    }

}; /* TFDialog */


#endif // TF_DIALOG_H
