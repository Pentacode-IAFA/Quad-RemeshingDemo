//
// Created by clement on 23/01/2022.
//

#ifndef PLUGINHOWTO_MAINAPPLICATION_H
#define PLUGINHOWTO_MAINAPPLICATION_H

#include <Gui/BaseApplication.hpp>

/// Allow singleton-like access to the main app à la qApp.
#if defined( mainApp )
#    undef mainApp
#endif
#define mainApp ( static_cast<Ra::MainApplication*>( qApp ) )

namespace Ra {
    class MainApplication : public Ra::Gui::BaseApplication
    {
    public:
        using Ra::Gui::BaseApplication::BaseApplication;
    };

} // namespace Ra


#endif //PLUGINHOWTO_MAINAPPLICATION_H
