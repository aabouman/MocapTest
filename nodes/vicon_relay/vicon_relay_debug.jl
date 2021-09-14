module ViconRelayDebug
    import Mercury as Hg
    using LibSerialPort
    using TOML

    # Import protobuf messages
    include("$(@__DIR__)/../../msgs/vicon_msg_pb.jl")
    include("$(@__DIR__)/../../msgs/messaging.jl")

    function vicon_relay(port_name::String, baudrate::Int;
                         freq::Int64=200, debug::Bool=false)
        sp = LibSerialPort.open(port_name, baudrate)
        close(sp)

        pub = Hg.SerialPublisher(sp);
        lrl = Hg.LoopRateLimiter(freq)

        vicon = VICON(pos_x=0., pos_y=0., pos_z=0.,
                      quat_w=0., quat_x=0., quat_y=0., quat_z=0.,
                      time=time())

        open(pub)
        try
            cnt = 1
            start_time = time()

            Hg.@rate while true
                vicon.pos_x, vicon.pos_y, vicon.pos_z = 0., 0., 0.
                vicon.quat_w, vicon.quat_x, vicon.quat_y, vicon.quat_z = 1., 0., 0., 0.
                vicon.time = time()

                Hg.publish(pub, vicon)

                num_cnt = 1000
                if cnt % num_cnt == 0 && debug
                    end_time = time()
                    println("Holybro publishing at: ", num_cnt / (end_time - start_time))
                    start_time = time()
                end
                cnt += 1
            end lrl
        catch e
            rethrow(e)
        finally
            close(pub)
        end
    end

    # Launch IMU publisher
    function main(; debug=false)
        setup_dict = TOML.tryparsefile("$(@__DIR__)/../setup.toml")

        serial_port = setup_dict["serial"]["ground"]["telemetry_radio"]["serial_port"]
        baud_rate = setup_dict["serial"]["ground"]["telemetry_radio"]["baud_rate"]

        # Launch the relay to send the Vicon data through the telemetry radio
        _vicon_relay() = vicon_relay(serial_port, baud_rate;
                                     freq=100, debug=debug)
        # _vicon_relay()
        return Threads.@spawn _vicon_relay()
    end
end
