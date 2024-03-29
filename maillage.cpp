#include "maillage.h"
#include <cmath>
//Constructeurs de maillage
Maillage::Maillage()
{
    nb_cellules=0;
}

Maillage::Maillage(double binf_x, double bsup_x,double binf_y,double bsup_y, int nb_cx,int nb_cy,int CondL)
{
    double dx;
    double dy;
    dx=(bsup_x-binf_x)/static_cast<double>(nb_cx);
    dy=(bsup_y-binf_y)/static_cast<double>(nb_cy);
    nb_celly=nb_cy;
    nb_cellx=nb_cx;
    nb_cellules=nb_cellx*nb_celly;
    CL=CondL;
    for(int j=0;j<nb_celly;j++){
      for (int i=0;i<nb_cellx;i++){
	Cellule *tmp =new Cellule(binf_x + i*dx,binf_x + (i+1)*dx,binf_y+j*dy,binf_y+(j+1)*dy,CL);
        m_V.push_back(tmp);
        if(i==nb_cellx-1){
	  tmp->setBsup_x(bsup_x);
        }
	if(j==nb_celly-1){
	  tmp->setBsup_y(bsup_y);
	}
      }
    }

}
Maillage :: Maillage(const Maillage &m){
    nb_cellules=m.nb_cellules;
    nb_celly=m.nb_celly;
    nb_cellx=m.nb_cellx;
}

//Ajoute une cellule au maillage
void Maillage::add_position(double valeur, int position)
{
    if(nb_cellules == 0)
    {
      Cellule *tmp = new Cellule(0., 1.,0.,1., valeur);
        m_V.push_back(tmp);
        nb_cellules++;
        return;
    }

    double inf_x = m_V.at(position)->getBinf_x();
    double mid_x = ( inf_x + m_V.at(position)->getBsup_x()) / 2.;
    double inf_y = m_V.at(position)->getBinf_y();
    double mid_y = ( inf_y + m_V.at(position)->getBsup_y()) / 2.;
    m_V.at(position)->setBinf_x(mid_x);

    Cellule *tmp = new Cellule(inf_x, mid_x,inf_y,mid_y ,valeur);

    m_V.insert(m_V.begin() + position, 1, tmp);
    nb_cellules++;
}

//Enleve une cellule au maillage
void Maillage::remove_position(int position)
{
    double inf = m_V.at(position)->getBinf_x();
    //Supprime la cellule allou�
    delete m_V.at(position);

    //Supprime la case du tableau
    m_V.erase( m_V.begin() + position);

    m_V.at(position)->setBinf_x(inf);
    nb_cellules--;
}

//retourne la valeur de la cellule ou -42 si la position n'est pas dans le maillage
double Maillage::operator[](int position)
{
    if( (position < 0) || (position >= nb_cellules) )
        return -42.;
    return m_V.at(position)->getValue();
}

// retourne le nombre de cellule du maillage
int Maillage::getNbCell() const
{
    return nb_cellules;
}

int Maillage::getNbCellx() const
{
    return nb_cellx;
}

int Maillage::getNbCelly() const
{
    return nb_celly;
}
//retourne la borne inf de la cellule i du maillage
double Maillage::getBinfCell_x(int i) const
{
    return m_V.at(i)->getBinf_x();
}

double Maillage::getBinfCell_y(int i) const
{
    return m_V.at(i)->getBinf_y();
}

//retourne la borne sup de la cellule i du maillage
double Maillage::getBsupCell_x(int i) const
{
    return m_V.at(i)->getBsup_x();
}

double Maillage::getBsupCell_y(int i) const
{
    return m_V.at(i)->getBsup_y();
}


//retourne la taille de la cellule i
double Maillage::getDxCell(int i) const
{
    return getBsupCell_x(i)-getBinfCell_x(i);
}

double Maillage::getDyCell(int i) const
{
    return getBsupCell_y(i)-getBinfCell_y(i);
}


//retourne la valeur de la cellule i
double Maillage::getValueCell(int i) const
{
    return m_V.at(i)->getValue();
}
double Maillage:: getValxy(int i,int j)const
{
  int i_tmp;
  int j_tmp;
  if(CL==0){
     if(i<0){
       i_tmp=i+getNbCellx();
     }
     else if(i>=getNbCellx()){
       i_tmp=i-getNbCellx();
     }
     else{
       i_tmp=i;
     }
     if(j<0){
       j_tmp=j+getNbCelly();
     }
     else if(j>=getNbCelly()){
       j_tmp=j-getNbCelly();
     }
     else{
       j_tmp=j;
     }
     j_tmp=j_tmp*getNbCellx();
  }
  else if (CL==1)
    { ///// pb sur les conditions de neumann!!!!
	  if(i>=getNbCellx()){
	    i_tmp=getNbCellx()-1;
	  }
	  else if(i<0){
	    i_tmp=0;
	    if( j>=0){
	      j_tmp=getNbCelly()-1;
	    }
	  }
	  else{
	    i_tmp=i;
	  }
	  if(j>=getNbCelly())
	    {
	      j_tmp=getNbCelly()-1;
	    }
	  else if(j<0)
	    {
	      j_tmp=0;
	      if(i==-1){
		i_tmp=0;
	      }
	      else if(i>=0){
		i_tmp=getNbCellx()-1;
	      }
	    }
	  else if(i>=0)
	    {
	      j_tmp=j;
	    }   
      j_tmp=j_tmp*getNbCellx();   
    }
  return m_V.at(i_tmp+j_tmp)->getValue();
}

//modifie la valeur de la cellule i
void Maillage::setValueCell(int i, double val)
{
    m_V.at(i)->setValue(val);
}

void Maillage::init_maill(int m_nFonc)
{
  switch(m_nFonc){
  case 0:
    setValueCell(0,0.5);
    for(int i=1; i<getNbCell();i++){
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if(mil_y<=0.5*mil_x){
	setValueCell(i,1);
      }
      else{
	setValueCell(i,0);
      }
    }
    break;
  case 1 :
    for(int i=0; i<getNbCell();i++){
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if(mil_y*mil_y+mil_x*mil_x<=0.2){
	setValueCell(i,1);
      }
      else{
	setValueCell(i,0);
      }
    }
    break;
  case 2:
    for(int i=0; i<getNbCell();i++){ 
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5-0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if(std::pow(mil_x,2)+std::pow(mil_y,2)<=0.2){
	setValueCell(i,1);	
      }
      else{
	setValueCell(i,0);
      }
    }
    break;
  case 3 :
    for(int i=0; i<getNbCell();i++){
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if((mil_y-0.5)*(mil_y-0.5)+(mil_x-0.75)*(mil_x-0.75)<=0.03){
	setValueCell(i,1);
      }
      else{
	setValueCell(i,0);
      }
    }
    break;
  case 4 :
    for(int i=0; i<getNbCell();i++){
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if(mil_y*mil_y+mil_x*mil_x<=0.03){
	setValueCell(i,0.2);
      }
      else if(mil_y*mil_y+mil_x*mil_x<=0.1){
	setValueCell(i,0.3);
      }
      else if(mil_y*mil_y+mil_x*mil_x<=0.5){
	setValueCell(i,0.5);
      }
      else{
	setValueCell(i,0);
      }
    }
    break;
  case 5 :
    for(int i=0; i<getNbCell();i++){
      double mil_x,mil_y;
      mil_x=getBinfCell_x(i)+getDxCell(i)*0.5;
      mil_y=getBinfCell_y(i)+getDyCell(i)*0.5;
      if(mil_y*mil_y+mil_x*mil_x<=0.3 && mil_x>0 && mil_y>0){
	setValueCell(i,0.2);
      }
      else if(mil_y*mil_y+mil_x*mil_x<=0.3 && mil_x>0 && mil_y<0 ){
	setValueCell(i,0.3);
      }
      else if(mil_y*mil_y+mil_x*mil_x<=0.3 && mil_x<0 && mil_y<0){
	setValueCell(i,0.1);
      }
      else if(mil_y*mil_y+mil_x*mil_x>0.3){
	setValueCell(i,0);
      }
      else{
	setValueCell(i,0.4);
      }
    }
    break;
  default:
    std::cout<<"mauvais choux de fonction"<<std::endl;
    break;
  }
}






// surcharge de l'operateur affichage
std::ostream &operator<<(std::ostream &os, const Maillage &M)
{
    for (int i=0; i<M.nb_cellules; ++i)
    {
        os << *M.m_V.at(i) << std::endl;
    }


    return os;
}
