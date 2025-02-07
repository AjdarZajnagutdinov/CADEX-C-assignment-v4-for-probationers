#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <ctime>
#include <random>
#include <chrono>
#include <omp.h>

class Point3D
{
public:
	double x;
	double y;
	double z;

	friend std::ostream& operator<< (std::ostream& out, const Point3D& point3D);
};

std::ostream& operator<< (std::ostream& out, const Point3D& point3D)
{
	out << "{ " << point3D.x << ", " << point3D.y << ", " << point3D.z << " }";

	return out;
}

class Curve3D
{
public:
	virtual Point3D get3DPoint(double t) = 0;

	virtual Point3D get3DVector(double t) = 0;

	virtual std::string getID() = 0;

	virtual ~Curve3D() {};
};

class Circle : public Curve3D
{
private:
	double R;
	std::string ID = "Circle";

public:
	Point3D get3DPoint(double t)
	{
		double x{ R * cos(t) };
		double y{ R * sin(t) };
		double z{ 0 };

		Point3D temp{ x, y, z };

		return temp;
	}

	Point3D get3DVector(double t)
	{
		double x{ -R * sin(t) };
		double y{ R * cos(t) };
		double z{ 0 };

		Point3D temp{ x, y, z };

		return temp;
	}

	double getR()
	{
		return R;
	}

	std::string getID()
	{
		return ID;
	}

	Circle(double R) try : R((R < 0) ? throw std::exception() : R)
						{}
						catch(std::exception& e)
						{ std::cout << "Error: radius is negative\n"; }
};

class Ellips : public Curve3D
{
private:
	double Rx;
	double Ry;
	std::string ID = "Ellips";

public:
	Point3D get3DPoint(double t)
	{
		double x{ Rx * cos(t) };
		double y{ Ry * sin(t) };
		double z{ 0 };

		Point3D temp{ x, y, z };

		return temp;
	}

	Point3D get3DVector(double t)
	{
		double x{ -Rx * sin(t) };
		double y{ Ry * cos(t) };
		double z{ 0 };

		Point3D temp{ x, y, z };

		return temp;
	}

	double getRx()
	{
		return Rx;
	}

	double getRy()
	{
		return Ry;
	}

	std::string getID()
	{
		return ID;
	}

	Ellips(double Rx, double Ry) try : Rx((Rx < 0) ? throw std::exception() : Rx),
										Ry((Ry < 0) ? throw std::exception() : Ry)
										{}
										catch (std::exception& e)
										{
											std::cout << "Error: radius is negative\n";
										}
};

class Helix : public Curve3D
{
private:
	double R;
	double step;
	std::string ID = "Helix";

public:
	Point3D get3DPoint(double t)
	{
		double x{ R * cos(t) };
		double y{ R * sin(t) };
		double z{ step * t / 8 / atan(1) };

		Point3D temp{ x, y, z };

		return temp;
	}

	Point3D get3DVector(double t)
	{
		double x{ -R * sin(t) };
		double y{ R * cos(t) };
		double z{ step / 8 / atan(1) };

		Point3D temp{ x, y, z };

		return temp;
	}

	double getR()
	{
		return R;
	}

	double getstep()
	{
		return step;
	}

	std::string getID()
	{
		return ID;
	}

	Helix(double R, double step) try : R((R < 0) ? throw std::exception() : R), step(step)
									{}
									catch (std::exception& e)
									{
										std::cout << "Error: radius is negative\n";
									}
};

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	rand();
	std::random_device rd;
	std::mt19937 mersenne(rd());

	int Vsz =20;

	double RCmax = 10;
	double RxEmax = 10;
	double RyEmax = 10;
	double RHmax = 10;
	double stepmin = -5;
	double stepmax = 5;
	
	double t = atan(1);



	std::vector<std::shared_ptr<Curve3D>> Vec;

	// 2. Заполнение вектора 3D-кривых
	for (int i = 0; i < Vsz; i++)
	{
		switch (rand() % 3)
		{
		case 0:
			Vec.push_back(std::shared_ptr<Curve3D>(new Circle(mersenne() / (4294967295/RCmax))));
			break;
		case 1:
			Vec.push_back(std::shared_ptr<Curve3D>(new Ellips(mersenne() / (4294967295/RxEmax), mersenne() / (4294967295/RyEmax))));
			break;
		case 2:
			Vec.push_back(std::shared_ptr<Curve3D>(new Helix(mersenne() / (4294967295/RHmax), mersenne() / (4294967295/(stepmax-stepmin)) + stepmin)));
			break;
		}
	}
	
	// 3. Вывод координат точек и производных всех кривых в контейнере при t=Pi/4
	std::cout << "3D-points coordinates:" << std::endl;
	std::for_each(Vec.begin(), Vec.end(), [&t](std::shared_ptr<Curve3D> val) {std::cout << (*val).get3DPoint(t) << " "; });
	std::cout << std::endl << std::endl;
	std::cout << "3D-vectors coordinates:" << std::endl;
	std::for_each(Vec.begin(), Vec.end(), [&t](std::shared_ptr<Curve3D> val) {std::cout << (*val).get3DVector(t) << " "; });
	std::cout << std::endl << std::endl;



	std::vector<std::shared_ptr<Curve3D>> Vec2;

	// 4. Заполнение второго контейнера окружностями из первого контейнера
	std::copy_if(Vec.begin(), Vec.end(), std::back_inserter(Vec2), [](std::shared_ptr<Curve3D> val) {return ((*val).getID() == "Circle"); });

	// Вывод адресов элементов двух контейнеров 
	// По совпадению адресов убеждаемся, что второй контейнер содержит указатели на элементы первого контейнера
	std::cout << "Addresses of elements of the first container:" << std::endl;
	std::copy(Vec.begin(), Vec.end(), std::ostream_iterator<std::shared_ptr<Curve3D>>(std::cout, " "));
	std::cout << std::endl << std::endl;
	std::cout << "Addresses of elements of the second container:" << std::endl;
	std::copy(Vec2.begin(), Vec2.end(), std::ostream_iterator<std::shared_ptr<Curve3D>>(std::cout, " "));
	std::cout << std::endl << std::endl;



	// 5. Сортировка второго контейнера в порядке возрастания радиусов окружностей
	std::sort(Vec2.begin(), Vec2.end(), [](std::shared_ptr<Curve3D> val1, std::shared_ptr<Curve3D> val2) 
										{return std::dynamic_pointer_cast<Circle>(val1)->getR() < std::dynamic_pointer_cast<Circle>(val2)->getR(); });

	// Вывод радиусов окружностей второго контейнера для проверки сортировки
	std::cout << "Radiuses of circles:" << std::endl;
	std::for_each(Vec2.begin(), Vec2.end(), [](std::shared_ptr<Curve3D> val) {std::cout << std::dynamic_pointer_cast<Circle>(val)->getR() << " "; });
	std::cout << std::endl << std::endl;



	double sumR = 0;

	// 6. Вычисление общей суммы радиусов всех кривых во втором контейнере
	const auto start{ std::chrono::steady_clock::now() };
	std::for_each(Vec2.begin(), Vec2.end(), [&sumR](std::shared_ptr<Curve3D> val) {sumR += std::dynamic_pointer_cast<Circle>(val)->getR(); });
	const auto end{ std::chrono::steady_clock::now() };
	const std::chrono::duration<double> elapsed_seconds{ end - start };

	// Вывод полученного значения суммы радиусов
	std::cout << "Serial version:" << std::endl;
	std::cout << "Sum of radiuses: " << sumR << std::endl;
	std::cout << "time " << elapsed_seconds.count() * 1000 << " ms" << std::endl << std::endl;



	double sumR2 = 0;
	int vs = Vec2.size();

	// 8. Вычисление общей суммы радиусов всех кривых во втором контейнере (параллельная версия)
	// Параллельная версия становится эффективной лишь при количестве элементов первого контейнера Vsz порядка 100000 
	// (у второго контейнера будет примерно треть от этого количества элементов)
	const auto start2{ std::chrono::steady_clock::now() };
	#pragma omp parallel reduction(+: sumR2)
	{
	#pragma omp for
		for (int i = 0; i < vs; ++i)
		{
			sumR2 += std::dynamic_pointer_cast<Circle>(Vec2[i])->getR();
		}
	}
	const auto end2{ std::chrono::steady_clock::now() };
	const std::chrono::duration<double> elapsed_seconds2{ end2 - start2 };

	// Вывод полученного значения суммы радиусов
	std::cout << "Parallel version:" << std::endl;
	std::cout << "Sum of radiuses: " << sumR2 << std::endl;
	std::cout << "time " << elapsed_seconds2.count() * 1000 << " ms" << std::endl << std::endl;



	return 0;
}