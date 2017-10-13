/**
 * @brief MLABTrajectory
 * @date Sep 26, 2017
 * @author a1994846931931
 */

#ifndef MLABTRAJECTORY_H_
#define MLABTRAJECTORY_H_

#include "Interpolator.h"
# include "../math/Q.h"
# include <memory>
# include "CircularInterpolator.h"
# include "LinearInterpolator.h"
# include "ConvertedInterpolator.h"
# include "Trajectory.h"

using robot::math::Q;
using std::vector;

namespace robot {
namespace trajectory {

/**
 * @brief 连续规划-多直线圆弧混合路径类
 *
 * 连续直线, 用圆弧来混合. n个路径点可以表示n-1条线段和n-2个圆弧. 这些圆弧和直线的路径
 * 分别保存在位置和姿态的插补器指针容器中, 同时重复保存在Trajectory指针容器中. 各路径
 * 的长度也得以保存. 各路径的lt也在容器中保存. 以上所谓的路径均以路径长度作为索引而建立,
 * 由路径(长度为索引)和lt(时间到长度的映射)统一指定的Q插补器亦在容器中保存.
 * 此外, 将所有路径统一起来保存在 _trajectory中, 所有lt统一起来保存在_lt中.
 * 所有私有变量以_v开头的表明它是一个容器.
 */
class MLABTrajectory: public Interpolator<Q> {
public:
	using ptr = std::shared_ptr<MLABTrajectory>;

	/**
	 * @brief 构造函数
	 * @param arcPosIpr [in] 圆弧段位置插补器列表
	 * @param linePosIpr [in] 直线段位置插补器列表
	 * @param arcRotIpr [in] 圆弧段姿态插补器列表
	 * @param lineRotIpr [in] 直线段姿态插补器列表
	 * @param length [in] 各段长度列表(包括直线段和圆弧段)
	 * @param qIpr [in] 各段的Q插补器列表(时间为索引, 包括直线段和圆弧段)
	 * @param trajectoryIpr [in] 各段的Q插补器列表(长度为索引, 包括直线段和圆弧段)
	 * @param lt [in] 各段的时间规划插补器列表(包括直线段和圆弧段)
	 * @param origin [in] 整条线段的位置和姿态插补器(时间为索引)
	 * @param iksolver [in] 用于逆解的逆解器
	 * @param config [in] 用于逆解的位姿参数
	 */
	MLABTrajectory(
			vector<CircularInterpolator<Vector3D<double> >::ptr> arcPosIpr,
			vector<LinearInterpolator<Vector3D<double> >::ptr> linePosIpr,
			vector<LinearInterpolator<Rotation3D<double> >::ptr> arcRotIpr,
			vector<LinearInterpolator<Rotation3D<double> >::ptr> lineRotIpr,
			vector<double> length,
			vector<Interpolator<Q>::ptr> qIpr,
			vector<Trajectory::ptr> trajectoryIpr,
			vector<SequenceInterpolator<double>::ptr > lt,
			std::pair<Interpolator<Vector3D<double> >::ptr , Interpolator<Rotation3D<double> >::ptr >  origin,
			std::shared_ptr<robot::ik::IKSolver> iksolver,
			robot::model::Config config);

	Q x(double t) const;
	Q dx(double t) const;
	Q ddx(double t) const;

	/**
	 * @brief 获取时间索引t处的路径长度
	 * @param t [in] 时间索引
	 * @return t处的路径长度
	 */
	double l(double t) const;

	/**
	 * @brief 获取时间索引t处的路径速度
	 * @param t [in] 时间索引
	 * @return t处的路径速度
	 */
	double dl(double t) const;

	/**
	 * @brief 获取时间索引t处的路径加速度
	 * @param t [in] 时间索引
	 * @return t处的路径加速度
	 */
	double ddl(double t) const;

	double duration() const;

	/**
	 * @brief 获取各段的位置插补器列表
	 * @return 各段的位置插补器列表
	 */
	vector<Interpolator<Vector3D<double> >::ptr> getPosIpr() const;
	virtual ~MLABTrajectory(){}
private:
	/** @brief 线段段数 n */
	int _size;

	/** 以下均以每一段的路径长度作为索引 */
	/** @brief 圆弧位置插补器 n-1 */
	vector<CircularInterpolator<Vector3D<double> >::ptr> _varcPosIpr;
	/** @brief 直线位置插补器 n */
	vector<LinearInterpolator<Vector3D<double> >::ptr> _vlinePosIpr;
	/** @brief 圆弧姿态插补器 n-1 */
	vector<LinearInterpolator<Rotation3D<double> >::ptr> _varcRotIpr;
	/** @brief 直线姿态插补器 n */
	vector<LinearInterpolator<Rotation3D<double> >::ptr> _vlineRotIpr;

	/** @brief 各段的长度 2n-1 */
	vector<double> _vlength;
	/** @brief 各段的ikInterpolator 长度为索引 */
	vector<Trajectory::ptr> _vtrajectoryIpr;
	/** @brief 各段的ikInterpolator 2n-1 时间为索引*/
	vector<Interpolator<Q>::ptr > _vqIpr;

	/** @brief l(t) 2n-1 */
	vector<SequenceInterpolator<double>::ptr > _vlt;
	/** @brief 时间索引 */
	vector<double> _vt;
	/** @brief 长度索引 */
	vector<double> _vl;
	/** @brief t(l) 2n-1 */
//	Interpolator<double>::ptr _tl;

	/**> 统一的lt */
	SequenceInterpolator<double>::ptr _lt;

	/**> 统一的路径 */
	Trajectory::ptr _trajectory;

};

/** @} */

} /* namespace trajectory */
} /* namespace robot */

#endif /* MLABTRAJECTORY_H_ */
