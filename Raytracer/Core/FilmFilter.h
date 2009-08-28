#ifndef FILMFILTER_H
#define FILMFILTER_H

class FilmFilter
  {
  public:  
    FilmFilter(const double &i_x_width, const double &i_y_width);

    virtual double Evaluate(const double &i_x, const double &i_y) const = 0;

    double GetXWidth() const;
    double GetYWidth() const;

    virtual ~FilmFilter() {}

  private:
    // not implemented
    FilmFilter();
    FilmFilter(const FilmFilter&);
    FilmFilter &operator=(const FilmFilter&);

  private:
    double m_x_width, m_y_width;
  };

class BoxFilter: public FilmFilter
  {
  public:
    BoxFilter(const double &i_x_width, const double &i_y_width);

    double Evaluate(const double &i_x, const double &i_y) const;
  };

#endif // FILMFILTER_H