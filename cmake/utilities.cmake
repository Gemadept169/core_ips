macro(install_core_ips_and_dependencies) 
    install(IMPORTED_RUNTIME_ARTIFACTS sot::sot_base sot::sot_factory sot::sot_siamrpn)
    install(IMPORTED_RUNTIME_ARTIFACTS opencv_core opencv_videoio opencv_imgproc opencv_imgcodecs)
    get_property(importedTargetsAfter DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
    # message("[${CMAKE_PROJECT_NAME}] Imported targets: ${importedTargetsAfter}")
    set(grpcImportedTargets "")
    foreach(target IN LISTS importedTargetsAfter)
    if(target MATCHES "^gRPC::" OR 
        target MATCHES "^absl::" OR
        target MATCHES "^protobuf::")
        get_target_property(targetType ${target} TYPE)
        if(targetType STREQUAL "SHARED_LIBRARY")
            list(APPEND grpcImportedTargets ${target})
        endif()
    endif()
    endforeach()
    install(IMPORTED_RUNTIME_ARTIFACTS ${grpcImportedTargets})
    install(IMPORTED_RUNTIME_ARTIFACTS Qt6::Core)
    install(DIRECTORY
        ${CORE_INTERNAL_PATH}/sot/data/
        DESTINATION data/sot/
    )
    install(DIRECTORY
        ${CMAKE_SOURCE_DIR}/configs
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    install(FILES
        ${CMAKE_SOURCE_DIR}/scripts/run.sh
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    install(TARGETS ${CMAKE_PROJECT_NAME}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    qt_generate_deploy_app_script(
        TARGET ${CMAKE_PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_TRANSLATIONS
    )
    install(SCRIPT ${deploy_script})
endmacro()