import { color } from "d3-color";
import { interpolateRgb } from "d3-interpolate";
import React, { Component } from "react";
import LiquidFillGauge from "react-liquid-gauge";


export default class LiquidChar extends Component {
  startColor = "green"; // cornflowerblue
  endColor = "green"; // crimson
  // componentDidMount(){
    
  // }

  render() {
    // console.log("valuess", this.props)
    const radius = 130;
    const interpolate = interpolateRgb(this.startColor, this.endColor);
    const fillColor = interpolate(this.props.value / 100);
    const gradientStops = [
      {
        key: "0%",
        stopColor: color(fillColor).darker(0.5).toString(),
        stopOpacity: 1,
        offset: "0%",
      },
      {
        key: "50%",
        stopColor: fillColor,
        stopOpacity: 0.75,
        offset: "50%",
      },
      {
        key: "100%",
        stopColor: color(fillColor).brighter(0.5).toString(),
        stopOpacity: 0.5,
        offset: "100%",
      },
    ];

    return (
      <div>
        <LiquidFillGauge
          width={radius * 2}
          height={radius * 2}
          value={ (((this.props?.data?.cell_rssi_dBm)/10 + 120) * 100) / 170 }
          percent=""
          textSize={1}
          textOffsetX={0}
          textOffsetY={0}
          textRenderer={(props) => {
            const value = Math.round((this.props?.data?.cell_rssi_dBm)/10);
            const radius = Math.min(props.height / 2, props.width / 2);
            const textPixels = (props.textSize * radius) / 2;
            const valueStyle = {
              fontSize: textPixels,
            };
            const percentStyle = {
              fontSize: textPixels * 0.6,
            };

            return (
              <tspan>
                <tspan className="value" style={valueStyle}>
                  {value}
                </tspan>
                <tspan style={percentStyle}>{props.percent}</tspan>
              </tspan>
            );
          }}
          riseAnimation
          waveAnimation
          waveFrequency={2}
          // minValue={-120}
          // maxValue={-50}
          waveAmplitude={1}
          gradient
          gradientStops={gradientStops}
          circleStyle={{
            fill: fillColor,
          }}
          waveStyle={{
            fill: fillColor,
          }}
          textStyle={{
            fill: color("#444").toString(),
            fontFamily: "Arial",
          }}
          waveTextStyle={{
            fill: color("#fff").toString(),
            fontFamily: "Arial",
          }}
        // onClick={() => {
        //   this.setState({ value: ((-80+120)*100)/170 });
        // }}
        />
        <div
          style={{
            margin: "auto",
            width: 120,
          }}
        >
          {/* <button
            type="button"
            className="btn btn-default btn-block"
            onClick={() => {
              this.setState({ value: Math.random() * 100 });
            }}
          >
            Refresh
          </button> */}
        </div>
      </div>
    );
  }
}
