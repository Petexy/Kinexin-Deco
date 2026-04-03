#include "kinexindecoration.h"
#include "kinexinbutton.h"

#include <KDecoration3/DecorationThemeProvider>
#include <KPluginFactory>

namespace Kinexin
{

class ThemeProvider : public KDecoration3::DecorationThemeProvider
{
    Q_OBJECT

public:
    explicit ThemeProvider(QObject *parent)
        : KDecoration3::DecorationThemeProvider(parent)
    {
    }

    QList<KDecoration3::DecorationThemeMetaData> themes() const override
    {
        KDecoration3::DecorationThemeMetaData data;
        data.setVisibleName(QStringLiteral("Kinexin Bar"));
        data.setThemeName(QStringLiteral("Kinexin"));
        data.setPluginId(QStringLiteral("kinexin-deco-kwin"));
        data.setConfigurationName(QStringLiteral("kcm_kinexindecoration"));
        data.setBorderSize(KDecoration3::BorderSize::None);
        return {data};
    }
};

} // namespace Kinexin

K_PLUGIN_FACTORY_WITH_JSON(
    KinexinDecoFactory,
    "metadata.json",
    registerPlugin<Kinexin::Decoration>();
    registerPlugin<Kinexin::ThemeProvider>();
)

#include "plugin.moc"
