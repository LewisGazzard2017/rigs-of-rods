
#include "SimulationG1.h"

#include "BeamFactory.h"

// ------------- PHYSICS UPDATE (w/THREADPOOL) --------------
//
// RoRFrameListener::frameStarted()
//   BeamFactory::SyncWithSimThread()
//     BeamFactory::m_sim_task.join()
//   BeamFactory::updateFlexbodiesPrepare()
//     FOR EACH TRUCK
//       Beam::updateFlexbodiesPrepare()
//         FOR EACH WHEEL // All wheel types are treated as flexbodies!
//           Flexable::flexitPrepare()
//         FOR EACH FLEXBODY
//           Flexable::flexitPrepare()
//         FOR EACH FLEXBODY
//           THREADPOOL <-- { Flexable::flexitCompute() }
//         FOR EACH WHEEL
//           THREADPOOL <-- { Flexable::flexitCompute() }
//   BeamFactory::updateVisual()
//     FOR EACH TRUCK
//       Beam::updateLabels() // multiplayer only
//       IF TRUCK STATE < SLEEPING
//         Beam::updateVisual()
//           Beam::autoBlinkReset()
//           Beam::updateSoundSources()
//             FOR EACH SOUNDSOURCE
//               SoundScriptInstance::setPosition()
//             SoundScriptManager::modulate(SS_MOD_AIRSPEED, SS_MOD_WHEELSPEED)
//           IF AIRPLANE && !SLEEPING && TIMER
//             SoundScriptManager::trigOnce(SS_TRIG_AVICHATTER)
//           FOR EACH CPARTICLE
//             Ogre::ParticleEmitter::setDirection()
//           FOR EACH EXHAUST
//             Ogre::SceneNode*; Ogre::ParticleEmitter*
//           Beam::updateProps()
//             FOR EACH PROP
//               Ogre::SceneNode*
//               IF PROP has WHEEL
//                 Ogre::SceneNode*
//             FOR EACH AIRBRAKE
//               Airbrake::updatePosition()
//           FOR EACH AEROENGINE
//             AeroEngine::updateVisuals()
//           FOR EACH WING
//             FlexAirfoil::setControlDeflection()
//           FOR EACH BEAM
//             Ogre::SceneNode*
//         Beam::updateSkidmarks()
//           FOR EACH WHEEL
//             Skidmark::update()
//         Beam::updateFlares()        
//           FOR EACH PROP
//             IF PROP IS BEACON
//               Ogre: Light, SceneNode, Quaternion...
//           FOR EACH FLARE
//             ...input, sounds, materials, SceneNode, Light...  
//     Mirrors::Update()          
//  BeamFactory::joinFlexbodyTasks()
//    FOR EACH TRUCK
//      Beam::joinFlexbodyTasks()
//  BeamFactory::update()
//    BeamFactory::SyncWithSimThread()
//      m_sim_task.join()
//    BeamFactory::UpdateSleepingState()
//    FOR EACH TRUCK
//      Beam::handleResetRequests()
//        IF RESET REQUESTED
//          Beam::SyncReset()
//      Beam::updateAngelScriptEvents()
//      IF VEHICLE HAS AI
//        VehicleAI::update()
//    IF TRUCK IS NETWORKED
//      Beam::calcNetwork()
//    ELSE
//      IF NOT SIMULATED
//        BeamEngine::update()
//      IF STATE < SLEEPING
//        Beam::UpdatePropAnimations()
//      IF NETWORKING
//        Beam::sendStreamData()
//    Beam::updateDashBoards()
//      IF !Beam::replayStep()
//        Beam::updateForceFeedback()
//        THREADPOOL <-- { Beam::UpdatePhysicsSimulation() }
//  BeamFactory::updateFlexbodiesFinal()
//    FOR EACH TRUCK
//      Beam::updateFlexbodiesFinal()
//        ... join tasks, Flexable::flexitFinal() ...

// ====== BeamFactory::UpdatePhysicsSimulation() ======
// FOR EACH TRUCK
//   Beam::preUpdatePhysics()
// FOR EACH PHYSICS_STEP
//   FOR EACH TRUCK
//     Beam::calcForcesEulerPrepare
//       Beam::forwardCommands()
//       Beam::calcBeamsInterTruck()
//     IF PREPARED
//       THREADPOOL <-- {
//         Beam::calcForcesEulerCompute
//         IF !DISABLE_TRUCK_TRUCK_SELF_COLLISIONS
//           PointColDetector::update()
//           intraTruckCollisions()
//         }
//   FOR EACH TRUCK
//     IF TRUCK SIMULATED
//       Beam::calcForcesEulerFinal()
//   FOR EACH TRUCK
//     IF TRUCK SIMULATED && !DISABLE_TRUCK_TRUCK_COLLISION
//       THREADPOOL <-- { PointColDetector::update(); interTruckCollisions() }
// FOR EACH TRUCK
//   Beam::postUpdatePhysics()

// ========== Beam::calcForcesEulerCompute() ==========
// BeamEngine::update()
// Beam::calcBeams()
// Beam::hookToggle()
// FOR EACH HOOK
//   ..timer..
// IF PLAYER CONTROLLED
//   FOR EACH HYDRO
//     ..update..
// Beam::updateSlideNodeForces()
// Beam::calcNodes()
// FOR EACH COLLISION_AABB
//   ..scale aabb to 0..
// FOR EACH NODE
//   ..update aabb..
// FOR EACH COLLISION_AABB
//   ..update predicted coll aabb...
// FOR EACH AEROENGINE
//   AeroEngine::updateForces()
// FOR EACH SCREWPROP
//   ScrewProp::updateForces()
// IF FUSE_AIRFOIL
//   ..calc fusedrag..
// FOR EACH AIRBRAKE
//   AirBrake::applyForce()
// FOR EACH BUOYCAB
//   Buoyance::computeNodeForce()
// IF AXLES
//   FOR EACH PROPED_WHEEL
//     ..update..
// FOR EACH AXLE
//   Axle::calcLockedDiff()
// FOR EACH AXLE
//   Axle::calcTorque()
// BeamEngine::getAcc()
// FOR EACH WHEEL
//   ..Update:wheel, alb, tc..
// BeamEngine::setSpin()
// IF STABILIZERS
//   FOR EACH SHOCK
//     ..update..
// IF HAS COMMANDS
//   BeamEngine*
//   FOR EACH COMMAND
//     ..update autoMoveLock..
//   FOR EACH COMMAND
//     ..updates..
//   FOR EACH COMMAND
//     ..normal command updates..
//   BeamEngine::setHydroPumpWork()
//   BeamEngine::setPrime()
//   FOR EACH ROTATOR
//     ..update..
//   FOR EACH TIE
//     ..process..
//   IF Replay::isValid()
//     Replay::getWriteBuffer()
//     Replay::writeDone()


void G1LogicContext::BeginUpdate(size_t dt_milis)
{
    ++m_num_frames;

    // COMPAT: Use float value
    float dt_sec = static_cast<float>(static_cast<double>(dt_milis) / 1000.0);
    dt_sec = std::min(dt_sec, 1.0f / 20.0f); // COMPAT: do not allow dt > 1/20
    dt_sec *= m_sim_speed;
    dt_sec += m_dt_remainder;

    m_update_num_steps = dt_sec / PHYSICS_DT;
    m_dt_remainder = dt_sec - (m_update_num_steps * PHYSICS_DT);
    dt_sec = PHYSICS_DT * m_update_num_steps;
}

// OLD: void Beam::preUpdatePhysics(float dt)
void G1Actor::BeginUpdate()
{
    m_prev_avg_pos = m_avg_pos; // TODO: Do it later when actually needed.

    if (this-> m_nodes[0].rel_pos.squaredLength() > 10000.0)
    {
        this->TranslateOrigin(m_nodes[0].rel_pos);
    }
}

void G1Actor::TranslateOrigin(Ogre::Vector3 offset)
{
    m_origin += offset;

    // TODO: Parallelize this
    size_t max = m_nodes.size();
    for (size_t i = 0; i < max; ++i)
    {
        m_nodes[i].rel_pos -= offset;
    }
}

void G1Actor::UpdateBeams()
{
    for (G1Beam& beam : m_beams_intra)
    {
        const Ogre::Vector3 distance = beam.p1->rel_pos - beam.p2->rel_pos;
        const float sq_length = distance.squaredLength();
        const float inv_length = fast_invSqrt(sq_length);
        const float cur_len = sq_length * inv_length;
        const float cur_len_diff = cur_len - beam.base_len;

        float spring = beam.spring;
        float damp = beam.damp;

        if (beam.is_shock1)
        {
            float interp_ratio;
            bool process = true;
            // ORIG: Following code interpolates between defined beam parameters and default beam parameters
            const float max_len = beam.long_bound * beam.base_len;
            const float min_len = beam.short_bound * beam.base_len;
            if (cur_len_diff > max_len)
                interp_ratio = cur_len_diff - max_len;
            else if (cur_len_diff < min_len)
                interp_ratio = -cur_len_diff - min_len;
            else
                process = false;

            if (process)
            {
                // ORIG: Hard (normal) shock bump
                float tspring = DEFAULT_SPRING;
                float tdamp = DEFAULT_DAMP;

                // ORIG: Skip camera, wheels or any other shocks which are not generated in a shocks or shocks2 section
                if (beam.is_hydro || beam.is_invis_hydro)
                {
                    tspring = beam.shock->sbd_spring;
                    tdamp = beam.shock->sbd_damp;
                }

                spring = (tspring - spring) * interp_ratio;
                damp = (tdamp - damp) * interp_ratio;
            }            
        }
        else if (beam.is_shock2)
        {

        }
    }
}




