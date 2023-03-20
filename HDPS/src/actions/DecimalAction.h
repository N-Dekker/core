#pragma once

#include "NumericalAction.h"

#include <QDoubleSpinBox>
#include <QSlider>
#include <QLineEdit>

class QWidget;
class QPushButton;

namespace hdps::gui {

/**
 * Decimal action class
 *
 * Stores a float value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalAction : public NumericalAction<float>
{
    Q_OBJECT

public:

    /** Spinbox widget class for decimal action */
    class SpinBoxWidget : public QDoubleSpinBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

    /** Slider widget class for decimal action */
    class SliderWidget : public QSlider
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SliderWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

    /** Line edit widget class for decimal action */
    class LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        LineEditWidget(QWidget* parent, DecimalAction* decimalAction);

    private:
        QDoubleValidator    _validator;

        friend class DecimalAction;
    };

protected:

    /**
     * Get widget representation of the decimal action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     * @param numberOfDecimals Number of decimals
     */
    DecimalAction(QObject * parent, const QString& title, const float& minimum = INIT_MIN, const float& maximum = INIT_MAX, const float& value = INIT_VALUE, const float& defaultValue = INIT_DEFAULT_VALUE, const std::uint32_t& numberOfDecimals = INIT_NUMBER_OF_DECIMALS);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the decimal action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     */
    void initialize(const float& minimum, const float& maximum, const float& value, const float& defaultValue, const std::uint32_t& numberOfDecimals = INIT_NUMBER_OF_DECIMALS);

    /** Gets the single step */
    float getSingleStep() const;

    /**
     * Sets the single step
     * @param singleStep Single step
     */
    void setSingleStep(const float& singleStep);

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the current value changed
     * @param value Current value that changed
     */
    void valueChanged(const float& value);

    /**
     * Signals that the default value changed
     * @param defaultValue Default value that changed
     */
    void defaultValueChanged(const float& defaultValue);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(const float& minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(const float& maximum);

    /**
     * Signals that the prefix changed
     * @param prefix New prefix
     */
    void prefixChanged(const QString& prefix);

    /**
     * Signals that the suffix changed
     * @param suffix New suffix
     */
    void suffixChanged(const QString& suffix);

    /**
     * Signals that the number of decimals changed to \p numberOfDecimals
     * @param numberOfDecimals Number of decimals
     */
    void numberOfDecimalsChanged(std::uint32_t numberOfDecimals);

    /**
     * Signals that the single step changed
     * @param singleStep Single step
     */
    void singleStepChanged(const float& singleStep);

protected:
    float   _singleStep;    /** Single step size for spin box */

protected:
    static constexpr float  INIT_MIN            = 0.0f;         /** Initialization minimum value */
    static constexpr float  INIT_MAX            = 100.0f;       /** Initialization maximum value */
    static constexpr float  INIT_VALUE          = 0.0;          /** Initialization value */
    static constexpr float  INIT_DEFAULT_VALUE  = 0.0;          /** Initialization default value */
    static constexpr int    INIT_DECIMALS       = 1;            /** Initialization number of decimals */
};

}
