#ifndef T3PROPHELPER_H
#define T3PROPHELPER_H

#include<QtCore>
#include <QtQml>

using MODU_ARGS_REF
	= const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	  , const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	  ,  QJsonArray*, QJsonArray*, QJsonArray*>*;

#define GET_TRACKVAR_F(ID,PROP,ARGREF) (*std::get<0>(*ARGREF))(ID,PROP, std::get<3>(*ARGREF))
#define GET_TRACKCON_F(ID,PROP,ARGREF) (*std::get<0>(*ARGREF))(ID,PROP, std::get<2>(*ARGREF))
#define GET_TRAIN_F(ID,PROP,ARGREF) (*std::get<0>(*ARGREF))(ID,PROP, std::get<4>(*ARGREF))
#define SET_TRACKVAR_F(ID,PROP,VALUE,ARGREF) (*std::get<1>(*ARGREF))(ID,PROP, std::get<3>(*ARGREF), VALUE)
#define SET_TRAIN_F(ID,PROP,VALUE,ARGREF) (*std::get<1>(*ARGREF))(ID,PROP, std::get<4>(*ARGREF), VALUE)

#define M2YARD_F(VAL) 1.90361f*static_cast<float>(VAL)
#define YARD2M_F(VAL) static_cast<float>(VAL)/1.90361f

#define KMH2MPH_F(VAL) 0.621371f*static_cast<float>(VAL)
#define MPH2KMH_F(VAL) static_cast<float>(VAL)/0.621371f

#define M2FOOT_F(VAL) 3.28084f*static_cast<float>(VAL)
#define FOOT2M_F(VAL) static_cast<float>(VAL)/3.28084f

#define FIREBASE_URL "https://sprn2023-ece1140-default-rtdb.firebaseio.com/"
#define INITIAL_LINE_CSV_DIR "C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/"

#define TRAINONBLOCK "KM_TRAINONBLOCK"




#endif // T3PROPHELPER_H
