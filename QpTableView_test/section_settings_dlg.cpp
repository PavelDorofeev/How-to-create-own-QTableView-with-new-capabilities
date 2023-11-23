#include "section_settings_dlg.h"
#include "ui_section_settings_dlg.h"
#include <QIntValidator>
#include <QDebug>
#include "app_def.h"

SectionSettingsDlg::SectionSettingsDlg(const qp::CELL_STYLE &stl ,
                                       const qp::CELL_STYLE &def,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SectionSettingsDlg),
    defaultStyle( def ),
    currStyles( stl )
{
    ui->setupUi(this);

    iVal.setRange(1,99);

    ui->ledt_fntWeight->setValidator( &iVal );

    ui->ledt_fntWeight->setText( QString::number( currStyles.font.weight() ) );

    if( currStyles.align & Qt::AlignLeft )
    {
        ui->chk_hor_left->setChecked( true );
    }
    else if( currStyles.align & Qt::AlignRight )
    {
        ui->chk_hor_right->setChecked( true );
    }
    else if( currStyles.align & Qt::AlignTop )
    {
        ui->chk_ver_top->setChecked( true );
    }
    else if( currStyles.align & Qt::AlignBottom )
    {
        ui->chk_ver_bottom->setChecked( true );
    }

    if( currStyles.align & Qt::AlignVCenter )
    {
        ui->chk_ver_top->setChecked( false );
        ui->chk_ver_bottom->setChecked( false );
    }

    if( currStyles.align & Qt::AlignHCenter )
    {
        ui->chk_hor_left->setChecked( false );
        ui->chk_hor_right->setChecked( false );
    }

    if( currStyles.align & Qt::AlignCenter )
    {

        ui->chk_center->setChecked( true );

    }

    // -------------------- colors -------------------------------

    colors.insert(ui->rbtn_blue , appDef::blue );
    colors.insert(ui->rbtn_green ,appDef::green );
    colors.insert(ui->rbtn_brown , appDef::brown );
    colors.insert(ui->rbtn_red , appDef::red );

    bool colorfnd = false;

    foreach( QRadioButton*  btn , colors.keys())
    {
        QString name = colors.value( btn ).name();
        btn->setStyleSheet("color:"+name+";");

        if( currStyles.color.name() == name )
        {
            btn->click();
            colorfnd = true;
        }
        else
            qDebug() << "colors: " << colors.value( btn ).name() << " != " << name;
    }


    if( !colorfnd )
    {
        ui->ledt_color->setText( currStyles.color.name() );

        ui->ledt_color->setStyleSheet("color:"+currStyles.color.name()+";");

        colors.insert(ui->rbtn_orig , currStyles.color );

        ui->rbtn_orig->click();
    }

    // ---------------------------------------------------

    defaultPointSz = defaultStyle.font.pointSize();
    defaultPixelSz = defaultStyle.font.pixelSize();

    currFnt = currStyles.font;

    qDebug()<< " currFnt " << currFnt;

    // ---------------------------------------------------

    sizes.insert( ui->rbtn_size_0_5 , 0.5);
    sizes.insert( ui->rbtn_size_0_7 , 0.7);
    sizes.insert( ui->rbtn_size_normal , 1);
    sizes.insert( ui->rbtn_size_1_5 , 1.5);
    sizes.insert( ui->rbtn_size_2_0 , 2.0);

    int pointSz = currStyles.font.pointSize();
    int pixelSz = currStyles.font.pixelSize();

    double kf=1;

    if( pixelSz >0 && defaultPixelSz>0 )
    {
        kf = (double)pixelSz / (double)defaultPixelSz ;
    }
    else if( pointSz >0 && defaultPointSz>0 )
    {
        kf = (double)pointSz / (double)defaultPointSz ;
    }

    foreach( QRadioButton*  btn , sizes.keys())
    {

        if( kf == sizes.value( btn ))
        {
            btn->click();

            break;
        }

    }

    // ----------------------- weights ----------------------------
    weights.insert( ui->chk_fnt_light , QFont::Light);
    weights.insert( ui->chk_fnt_normal , QFont::Normal);
    weights.insert( ui->chk_fnt_demibold , QFont::DemiBold);
    weights.insert( ui->chk_fnt_bold , QFont::Bold);
    weights.insert( ui->chk_fnt_black , QFont::Black);

    ui->chk_fnt_light->setProperty("weight" , QFont::Light);
    ui->chk_fnt_normal->setProperty("weight" , QFont::Normal);
    ui->chk_fnt_demibold->setProperty("weight" , QFont::DemiBold);
    ui->chk_fnt_bold->setProperty("weight" , QFont::Bold);
    ui->chk_fnt_black->setProperty("weight" , QFont::Black);

    foreach( QRadioButton*  btn , weights.keys())
    {

        if( currStyles.font.weight() == btn->property("weight").toInt() )
        {
            qDebug() << "weights: " << weights.value( btn ) << " == " << btn->property("weight").toInt();

            btn->click();

            break;
        }
        else
            qDebug() << "weights: " << weights.value( btn ) << " != " << btn->property("weight").toInt();
    }

    // ---------------------------------------------------

}

SectionSettingsDlg::~SectionSettingsDlg()
{
    delete ui;
}


void SectionSettingsDlg::on_chk_hor_left_toggled(bool checked)
{

}

void SectionSettingsDlg::on_chk_hor_left_clicked(bool checked)
{
    if( checked )
    {
        ui->chk_hor_right->setChecked( false );

        if( isVertAlign() )
            ui->chk_center->setChecked( false );

        recalculate_align();
    }

}

void SectionSettingsDlg::on_chk_hor_right_clicked(bool checked)
{

    if( checked )
    {
        ui->chk_hor_left->setChecked( false );

        if( isVertAlign() )
            ui->chk_center->setChecked( false );

        recalculate_align();

    }
}

void SectionSettingsDlg::on_chk_ver_top_clicked(bool checked)
{

    if( checked )
    {
        ui->chk_ver_bottom->setChecked( false );

        if( isHorAlign() )
            ui->chk_center->setChecked( false );

        recalculate_align();
    }
}

void SectionSettingsDlg::on_chk_ver_bottom_clicked(bool checked)
{
    if( checked )
    {
        ui->chk_ver_top->setChecked( false );

        if( isHorAlign() )
            ui->chk_center->setChecked( false );

        recalculate_align();
    }

}

void SectionSettingsDlg::on_chk_center_clicked(bool checked)
{
    if( checked )
    {
        if( isVertAlign() && isHorAlign())
        {
            ui->chk_ver_top->setChecked( false );
            ui->chk_ver_bottom->setChecked( false );
            ui->chk_hor_left->setChecked( false );
            ui->chk_hor_right->setChecked( false );
        }
        else if( isVertAlign() )
        {
            ui->chk_hor_left->setChecked( false );
            ui->chk_hor_right->setChecked( false );

        }
        else if( isHorAlign() )
        {
            ui->chk_ver_top->setChecked( false );
            ui->chk_ver_bottom->setChecked( false );

        }

        recalculate_align();

    }
}

bool SectionSettingsDlg::isHorAlign()
{
    if( ui->chk_hor_left->isChecked()
            || ui->chk_hor_right->isChecked() )
        return true;

    return false;

}
bool SectionSettingsDlg::isVertAlign()
{
    if( ui->chk_ver_top->isChecked()
            || ui->chk_ver_bottom->isChecked() )
        return true;

    return false;

}

void SectionSettingsDlg::recalculate_font()
{
    currStyles.font.setWeight ( ui->ledt_fntWeight->text().toInt() );
}

void SectionSettingsDlg::recalculate_align()
{
    if( ui->chk_hor_left->isChecked() )
    {
        currStyles.align |= Qt::AlignLeft;
        currStyles.align &= ~Qt::AlignRight;
        currStyles.align &= ~Qt::AlignHCenter;
    }
    else if( ui->chk_hor_right->isChecked() )
    {
        currStyles.align |= Qt::AlignRight;
        currStyles.align &= ~Qt::AlignLeft;
        currStyles.align &= ~Qt::AlignHCenter;
    }
    else if( ui->chk_center->isChecked() )
    {
        currStyles.align |= Qt::AlignHCenter;
        currStyles.align &= ~Qt::AlignRight;
        currStyles.align &= ~Qt::AlignLeft;
    }

    if( ui->chk_ver_top->isChecked() )
    {
        currStyles.align |= Qt::AlignTop;
        currStyles.align &= ~Qt::AlignBottom;
        currStyles.align &= ~Qt::AlignVCenter;
    }
    else if( ui->chk_ver_bottom->isChecked() )
    {
        currStyles.align |= Qt::AlignBottom;
        currStyles.align &= ~Qt::AlignTop;
        currStyles.align &= ~Qt::AlignVCenter;
    }
    else if( ui->chk_center->isChecked() )
    {
        currStyles.align |= Qt::AlignVCenter;
        currStyles.align &= ~Qt::AlignTop;
        currStyles.align &= ~Qt::AlignBottom;
    }

}

void SectionSettingsDlg::on_ledt_fntWeight_textEdited(const QString &arg1)
{
    currStyles.font.setWeight ( ui->ledt_fntWeight->text().toInt() );

}

void SectionSettingsDlg::on_buttonBox_accepted()
{
    recalculate_align();
    recalculate_font();

    accept();
}


void SectionSettingsDlg::on_chk_fnt_light_clicked(bool checked)
{
    ui->ledt_fntWeight->setText( QString::number(QFont::Light ) );
}


void SectionSettingsDlg::on_chk_fnt_normal_clicked(bool checked)
{
    ui->ledt_fntWeight->setText( QString::number(QFont::Normal ) );
}


void SectionSettingsDlg::on_chk_fnt_bold_clicked(bool checked)
{
    ui->ledt_fntWeight->setText( QString::number(QFont::Bold ) );
}

void SectionSettingsDlg::on_chk_fnt_demibold_clicked(bool checked)
{
    ui->ledt_fntWeight->setText( QString::number(QFont::DemiBold ) );
}

void SectionSettingsDlg::on_chk_fnt_black_clicked(bool checked)
{
    ui->ledt_fntWeight->setText( QString::number(QFont::Black ) );
}


void SectionSettingsDlg::on_rbtn_brown_clicked(bool checked)
{
    setColor();
}

void SectionSettingsDlg::on_rbtn_red_clicked(bool checked)
{
    setColor();
}

void SectionSettingsDlg::on_rbtn_green_clicked(bool checked)
{
    setColor();
}

void SectionSettingsDlg::on_rbtn_blue_clicked(bool checked)
{
    setColor();
}

void SectionSettingsDlg::on_rbtn_orig_clicked()
{
    setColor();
}

void SectionSettingsDlg::setColor()
{
    QRadioButton * btn = qobject_cast<QRadioButton *>(sender());

    if( btn)
    {
        QPalette palette = btn->palette();

        currStyles.color = palette.color( QPalette::Text );

        ui->ledt_color->setText( colors.value( btn).name() );

        //qDebug() << " currStyles.color.name() " << currStyles.color.name();
    }
}


void SectionSettingsDlg::setFontSize(double kf)
{
    QRadioButton * btn = qobject_cast<QRadioButton *>(sender());

    if( btn )
    {
        int pointSz=-1;
        int pixelSz=-1;

        if( defaultPointSz >0)
            pointSz = defaultPointSz * kf;

        if( defaultPixelSz >0)
            pixelSz = defaultPixelSz * kf;

        currStyles.font.setPointSize( pointSz );
        currStyles.font.setPixelSize ( pixelSz );

        ui->spb_pointSize->setValue( pointSz );
        ui->spb_pixelSize->setValue( pixelSz );
    }
}

void SectionSettingsDlg::on_rbtn_size_normal_clicked()
{

    setFontSize( 1 );
}


void SectionSettingsDlg::on_rbtn_size_0_7_clicked(bool checked)
{

    setFontSize( 0.7 );
}

void SectionSettingsDlg::on_rbtn_size_1_5_clicked(bool checked)
{
    setFontSize(  1.5);
}

void SectionSettingsDlg::on_spb_pointSize_valueChanged(int arg1)
{
    if( arg1 >0 )
        currStyles.font.setPointSize( arg1);
}

void SectionSettingsDlg::on_spb_pixelSize_valueChanged(int arg1)
{
    if( arg1 >0 )
        currStyles.font.setPixelSize( arg1);
}

void SectionSettingsDlg::on_rbtn_size_2_0_clicked()
{
    setFontSize( 2 );
}


void SectionSettingsDlg::on_rbtn_size_0_5_clicked(bool checked)
{
    setFontSize( 0.5 );
}


void SectionSettingsDlg::on_ledt_color_editingFinished()
{
    QString txt = ui->ledt_color->text();

    qDebug() << "on_ledt_color_textEdited " << txt << " == " << currStyles.color.name();

    currStyles.color.setNamedColor( txt );

    qDebug() << "on_ledt_color_textEdited " << txt << " == " << currStyles.color.name();

}

