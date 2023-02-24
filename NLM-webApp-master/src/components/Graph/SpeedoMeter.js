import React from "react";
import ReactCanvasGauge from "./ReactCanvus";

export default class SpeedoMeter extends React.Component {
  // constructor(props) {
  //   super(props);
  //   // this.state = { intervalHandler: -1, value: 0 };
  // }

  // componentWillMount() {
  //   // const intervalHandler = window.setInterval(() => {
  //     const value = this.props.cfo_ppb;

  //     this.setState({ value });
  //   // }, 5000);
  //   // this.setState({ intervalHandler });
  // }

  // componentWillUnmount() {
  //   window.clearInterval(this.state.intervalHandler);
  // }
  render() {
    return (
      <div>
        <ReactCanvasGauge
          key="radialGauge"
          width={250}
          height={250}
          value={Math.round((this.props?.data?.cfo_ppb)/1000)}
          gaugeType="RADIAL"
          minorTicks={9}
          majorTicks={[0, 10, 20, 30, 40, 50, 60, 70, 80, 90]}
          title=""
          units="Mb"
          needleType="arrow"
          highlights={[
            { from: 0, to: 40, color: "rgba(0,255,0,.15)" },
            { from: 40, to: 80, color: "yellow" },
            { from: 80, to: 100, color: "red" },
          ]}
          // colorNeedle="black"
          // colorNeedleCircleOuter="yellow"
          // colorNeedleCircleOuterEnd="blue"
          // colorNeedleCircleInner="black"
          // colorNeedleCircleInnerEnd="white"
          colorBorderOuter="black"
          // colorBorderOuterEnd="#111"
          // colorBorderMiddle="green"
          // colorBorderMiddleEnd="#111"
          // colorBorderInner="yellow"
          // colorBorderInnerEnd="green"

          //   highlights='[
          //     {"from": 0, "to": 42, "color": "red"},
          //     {"from": 0, "to": 100, "color": "green"}
          // ]'
          //   colorMajorTicks="black"
          //   colorMinorTicks="yellow"
        />
      </div>
    );
  }
}
