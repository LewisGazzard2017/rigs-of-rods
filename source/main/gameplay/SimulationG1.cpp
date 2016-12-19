
#include "SimulationG1.h"

#include "BeamFactory.h"

// NOTE: This code is largely cloned from original implementation (with cleanup)
//       Orig comments were preserved, new comments start with NEXTSIM

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

    // NEXTSIM|COMPAT: Use float value
    float dt_sec = static_cast<float>(static_cast<double>(dt_milis) / 1000.0);
    dt_sec = std::min(dt_sec, 1.0f / 20.0f); // COMPAT: do not allow dt > 1/20
    dt_sec *= m_sim_speed;
    dt_sec += m_dt_remainder;

    m_update_num_steps = dt_sec / PHYSICS_DT;
    m_dt_remainder = dt_sec - (m_update_num_steps * PHYSICS_DT);
    dt_sec = PHYSICS_DT * m_update_num_steps;
}

// NEXTSIM|OLD: void Beam::preUpdatePhysics(float dt)
void G1Actor::BeginUpdate()
{
    m_prev_avg_pos = m_avg_pos; // NEXTSIM|TODO: Do it later when actually needed.

    if (this-> m_nodes[0].rel_pos.squaredLength() > 10000.0)
    {
        this->TranslateOrigin(m_nodes[0].rel_pos);
    }
}

void G1Actor::TranslateOrigin(Ogre::Vector3 offset)
{
    m_origin += offset;

    // NEXTSIM|TODO: Parallelize this
    size_t max = m_nodes.size();
    for (size_t i = 0; i < max; ++i)
    {
        m_nodes[i].rel_pos -= offset;
    }
}

void G1Actor::UpdateBeams()
{
    for (G1Beam& beam : m_beams)
    {
        if (beam.is_disabled)
        {
            continue;
        }

        const Ogre::Vector3 distance = beam.p1->rel_pos - beam.p2->rel_pos;
        const float sq_length = distance.squaredLength();
        const float inv_length = fast_invSqrt(sq_length);
        const float cur_len = sq_length * inv_length;
        const float cur_len_diff = cur_len - beam.length;

        float spring = beam.spring;
        float damp = beam.damp;

        if (!beam.is_inter_actor)
        {
            if (beam.is_shock1)
            {
                G1Actor::UpdateBeamShock1(beam, cur_len_diff, spring, damp);            
            }
            else if (beam.is_shock2 && beam.shock != nullptr)
            {
                // NEXTSIM|TODO
            }
            else if (beam.is_support && cur_len_diff > 0.f)
            {
                spring = 0.f;
                damp *= 0.1f;
                // If this is a supportbeam with a user set break limit, get the user set limit
                const float break_limit = (beam.long_bound > 0.f) ? beam.long_bound : SUPPORT_BEAM_LIMIT_DEFAULT;

                // If support beam is extended the originallength * break_limit, break and disable it
                if (cur_len_diff > (beam.length * break_limit))
                {
                    beam.is_broken = true;
                    beam.is_disabled = true;
                }
            }
        }
        else if (beam.is_rope && cur_len_diff < 0.f)
        {
            spring = 0.f;
            damp *= 0.1f;
        }

        // Calculate beam's rate of change
        // NEXTSIM: not sure what 'slen' means ~ only_a_ptr, 12/2016    
        const Ogre::Vector3 v = beam.p1->velocity - beam.p2->velocity;
        float slen = -spring * cur_len_diff - damp * v.dotProduct(distance) * inv_length;
        beam.stress = slen;

        // Fast test for deformation
        float len = std::abs(slen); // NEXTSIM: kept old name 'len' (not sure what 'slen' is). ~ only_a_ptr, 12/2016    
        if (len > beam.deform_thr_abs)
        {
            this->UpdateBeamDeform(beam, slen, len, cur_len_diff, spring);
        }

        // Finally update beam forces
        const Ogre::Vector3 f = distance * (slen * inv_length);
        beam.p1->forces += f;
        beam.p2->forces -= f;
    }
}

void G1Actor::UpdateBeamDeform(G1Beam& beam, float& slen, float len, const float cur_len_diff, const float spring)
{
    if ((beam.is_normal || beam.is_invis) && !beam.is_shock1 && (spring != 0.f))
    {
        const bool compress = (slen > beam.deform_thr_compress && cur_len_diff < 0.f);
        const bool expand   = (slen < beam.deform_thr_expand   && cur_len_diff > 0.f);
        const float thr = (compress) ? beam.deform_thr_compress : beam.deform_thr_expand;
        
        if (compress || expand)
        {
            if (!beam.is_inter_actor)
            {
                m_step_context.increase_coll_accuracy = true;
            }
            const float deform = cur_len_diff + (thr / spring) * (1.f - beam.plastic_coef);
            const float old_len = beam.length;
            beam.length += deform;
            slen = slen - (slen - thr) * 0.5f;

            if (compress)
            {
                len = slen;
                beam.length = std::max(MIN_BEAM_LENGTH, beam.length);
                if ((beam.length > 0.f) && (old_len > beam.length))
                {
                    beam.deform_thr_compress *= old_len / beam.length;
                    const float deform_tmp = std::min(beam.deform_thr_compress, -beam.deform_thr_expand);
                    beam.deform_thr_abs = std::min(deform_tmp, beam.strength);
                }
            }
            else // expand
            {
                len = -slen;
                if ((old_len > 0.f) && (beam.length > old_len))
                {
                    beam.deform_thr_expand *= beam.length / old_len;
                    const float deform_tmp = std::min(beam.deform_thr_compress, -beam.deform_thr_expand);
                    beam.deform_thr_abs = std::min(deform_tmp, beam.strength);
                }
                beam.strength -= deform * spring;
            }
        }

        if (len > beam.strength)
        {
            this->UpdateBeamBreaking(beam, slen);
        }
    }
}

void G1Actor::UpdateBeamBreaking(G1Beam& beam, float& slen)
{
    if (!beam.is_inter_actor)
    {
        m_step_context.increase_coll_accuracy = true;
    }

    //Break the beam only when it is not connected to a node
    //which is a part of a collision triangle and has 2 "live" beams or less connected to it.

    const bool unable1 = beam.p1->is_contacter && m_softbody_graph.GetNumActiveNeighborBeams(beam.p1->pos) < 3;
    const bool unable2 = beam.p2->is_contacter && m_softbody_graph.GetNumActiveNeighborBeams(beam.p2->pos) < 3;
    if (!unable1 && !unable2)
    {
        slen = 0.f;
        beam.is_disabled = true;
        beam.is_broken = true;

        // NEXTSIM|TODO: detacher groups (disabled for inter-actor beams)
    }
    else
    {
        beam.strength = 2.f * beam.deform_thr_abs;
    }

    // NEXTSIM|TODO: check buoyant hull (disabled for inter-actor beams)
}

// static
void G1Actor::UpdateBeamShock1(G1Beam& beam, float cur_len_diff, float& spring, float& damp)
{
    float interp_ratio;
    bool process = true;
    // Following code interpolates between defined beam parameters and default beam parameters
    const float max_len = beam.long_bound * beam.length;
    const float min_len = beam.short_bound * beam.length;
    if (cur_len_diff > max_len)
        interp_ratio = cur_len_diff - max_len;
    else if (cur_len_diff < min_len)
        interp_ratio = -cur_len_diff - min_len;
    else
        process = false;

    if (process)
    {
        // Hard (normal) shock bump
        float tspring = DEFAULT_SPRING;
        float tdamp = DEFAULT_DAMP;

        // Skip camera, wheels or any other shocks which are not generated in a shocks or shocks2 section
        if (beam.is_hydro || beam.is_invis_hydro)
        {
            tspring = beam.shock->sbd_spring;
            tdamp = beam.shock->sbd_damp;
        }

        spring = (tspring - spring) * interp_ratio;
        damp = (tdamp - damp) * interp_ratio;
    }
}

void G1Actor::UpdateBeamShock2(G1Beam& beam, float cur_len_diff, float& k, float& d)
{
    const float beams_lep = beam.length * 0.8f; // ORIG: name 'beamsLep' - meaning??
    const float longbound_prelimit = beam.long_bound * beams_lep;
    const float shortbound_prelimit = beam.short_bound * beams_lep;

    float logafactor;
    //shock extending since last cycle
    if (beam.shock->last_len_diff < cur_len_diff)
    {
        //get outbound values
        k = beam.shock->spring_out;
        d = beam.shock->damp_out;
        // add progression
        if (beam.long_bound != 0.0f)
        {
            logafactor = cur_len_diff / (beam.long_bound * beam.length);
            logafactor = logafactor * logafactor;
        }
        else
        {
            logafactor = 1.0f;
        }
        if (logafactor > 1.0f)
            logafactor = 1.0f;
        k = k + (beam.shock->spring_out_prog * k * logafactor);
        d = d + (beam.shock->damp_out_prog * d * logafactor);
    }
    else
    {
        //shock compresssing since last cycle
        //get inbound values
        k = beam.shock->spring_in;
        d = beam.shock->damp_in;
        // add progression
        if (beam.short_bound != 0.0f)
        {
            logafactor = cur_len_diff / (beam.short_bound * beam.length);
            logafactor = logafactor * logafactor;
        }
        else
        {
            logafactor = 1.0f;
        }
        if (logafactor > 1.0f)
            logafactor = 1.0f;
        k = k + (beam.shock->spring_in_prog * k * logafactor);
        d = d + (beam.shock->damp_in_prog * d * logafactor);
    }
    if (beam.shock->is_soft_bump)
    {
        // soft bump shocks
        if (cur_len_diff > longbound_prelimit)
        {
            //reset to long_bound progressive values (oscillating beam workaround)
            k = beam.shock->spring_out;
            d = beam.shock->damp_out;
            // add progression
            if (beam.long_bound != 0.0f)
            {
                logafactor = cur_len_diff / (beam.long_bound * beam.L);
                logafactor = logafactor * logafactor;
            }
            else
            {
                logafactor = 1.0f;
            }
            if (logafactor > 1.0f)
                logafactor = 1.0f;
            k = k + (beam.shock->spring_out_prog * k * logafactor);
            d = d + (beam.shock->damp_out_prog * d * logafactor);
            //add shortbump progression
            if (beam.long_bound != 0.0f)
            {
                logafactor = ((cur_len_diff - longbound_prelimit) * 5.0f) / (beam.long_bound * beam.length);
                logafactor = logafactor * logafactor;
            }
            else
            {
                logafactor = 1.0f;
            }
            if (logafactor > 1.0f)
                logafactor = 1.0f;
            k = k + (k + 100.0f) * beam.shock->spring_out_prog * logafactor;
            d = d + (d + 100.0f) * beam.shock->damp_out_prog * logafactor;
            if (beam.shock->last_len_diff > cur_len_diff)
            // rebound mode..get new values
            {
                k = beam.shock->spring_in;
                d = beam.shock->damp_in;
            }
        }
        else if (cur_len_diff < shortbound_prelimit)
        {
            //reset to short_bound progressive values (oscillating beam workaround)
            k = beam.shock->spring_in;
            d = beam.shock->damp_in;
            if (beam.short_bound != 0.0f)
            {
                logafactor = cur_len_diff / (beam.short_bound * beam.L);
                logafactor = logafactor * logafactor;
            }
            else
            {
                logafactor = 1.0f;
            }
            if (logafactor > 1.0f)
                logafactor = 1.0f;
            k = k + (beam.shock->spring_in_prog * k * logafactor);
            d = d + (beam.shock->damp_in_prog * d * logafactor);
            //add shortbump progression
            if (beam.short_bound != 0.0f)
            {
                logafactor = ((cur_len_diff - shortbound_prelimit) * 5.0f) / (beam.short_bound * beam.length);
                logafactor = logafactor * logafactor;
            }
            else
            {
                logafactor = 1.0f;
            }
            if (logafactor > 1.0f)
                logafactor = 1.0f;
            k = k + (k + 100.0f) * beam.shock->spring_out_prog * logafactor;
            d = d + (d + 100.0f) * beam.shock->damp_out_prog * logafactor;
            if (beam.shock->last_len_diff < cur_len_diff)
            // rebound mode..get new values
            {
                k = beam.shock->spring_out;
                d = beam.shock->damp_out;
            }
        }
        if (cur_len_diff > beam.long_bound * beam.L || cur_len_diff < -beam.short_bound * beam.L)
        {
            // block reached...hard bump in soft mode with 4x default damp_ing
            if (k < beam.shock->sbd_spring)
                k = beam.shock->sbd_spring;
            if (d < beam.shock->sbd_damp)
                d = beam.shock->sbd_damp;
        }
    }

    if (beam.shock->is_normal)
    {
        if (cur_len_diff > beam.long_bound * beam.length || cur_len_diff < -beam.short_bound * beam.length)
        {
            if (beam.shock && !(beam.shock->is_trigger)) // this is NOT a trigger beam
            {
                // hard (normal) shock bump
                k = beam.shock->sbd_spring;
                d = beam.shock->sbd_damp;
            }
        }

        if (beam.shock && (beam.shock->is_trigger) && beam.shock->is_trigger_enabled) // this is a trigger and it's enabled
        {
            if (cur_len_diff > beam.long_bound * beam.length || cur_len_diff < -beam.short_bound * beam.length) // that has hit boundary
            {
                beam.shock->trigger_switch_state -= PHYSICS_DT;
                if (beam.shock->trigger_switch_state <= 0.0f) // emergency release for dead-switched trigger
                    beam.shock->trigger_switch_state = 0.0f;
                if (beam.shock->is_trig_blocker) // this is an enabled blocker and past boundary
                {
                    for (int scount = i + 1; scount <= i + beam.shock->trigger_cmdshort; scount++) // (cycle blockerbeamID +1) to (blockerbeamID + beams to lock)
                    {
                        if (beams[scount].shock && (beams[scount].shock->flags & SHOCK_FLAG_ISTRIGGER)) // don't mess anything up if the user set the number too big
                        {
                            if (triggerdebug && !beams[scount].shock->trigger_enabled && beam.shock->last_debug_state != 1)
                            {
                                LOG(" Trigger disabled. Blocker BeamID " + TOSTRING(i) + " enabled trigger " + TOSTRING(scount));
                                beam.shock->last_debug_state = 1;
                            }
                            beams[scount].shock->trigger_enabled = false; // disable the trigger
                        }
                    }
                }
                else if (beam.shock->flags & SHOCK_FLAG_TRG_BLOCKER_A) // this is an enabled inverted blocker and inside boundary
                {
                    for (int scount = i + 1; scount <= i + beam.shock->trigger_cmdlong; scount++) // (cycle blockerbeamID + 1) to (blockerbeamID + beams to release)
                    {
                        if (beams[scount].shock && (beams[scount].shock->flags & SHOCK_FLAG_ISTRIGGER)) // don't mess anything up if the user set the number too big
                        {
                            if (triggerdebug && beams[scount].shock->trigger_enabled && beam.shock->last_debug_state != 9)
                            {
                                LOG(" Trigger enabled. Inverted Blocker BeamID " + TOSTRING(i) + " disabled trigger " + TOSTRING(scount));
                                beam.shock->last_debug_state = 9;
                            }
                            beams[scount].shock->trigger_enabled = true; // enable the triggers
                        }
                    }
                }
                else if (beam.shock->flags & SHOCK_FLAG_TRG_CMD_BLOCKER) // this an enabled cmd-key-blocker and past a boundary
                {
                    commandkey[beam.shock->trigger_cmdshort].trigger_cmdkeyblock_state = false; // Release the cmdKey
                    if (triggerdebug && beam.shock->last_debug_state != 2)
                    {
                        LOG(" F-key trigger block released. Blocker BeamID " + TOSTRING(i) + " Released F" + TOSTRING(beam.shock->trigger_cmdshort));
                        beam.shock->last_debug_state = 2;
                    }
                }
                else if (beam.shock->flags & SHOCK_FLAG_TRG_CMD_SWITCH) // this is an enabled cmdkey switch and past a boundary
                {
                    if (!beam.shock->trigger_switch_state)// this switch is triggered first time in this boundary
                    {
                        for (int scount = 0; scount < free_shock; scount++)
                        {
                            int short1 = beams[shocks[scount].beamid].shock->trigger_cmdshort; // cmdshort of checked trigger beam
                            int short2 = beam.shock->trigger_cmdshort; // cmdshort of switch beam
                            int long1 = beams[shocks[scount].beamid].shock->trigger_cmdlong; // cmdlong of checked trigger beam
                            int long2 = beam.shock->trigger_cmdlong; // cmdlong of switch beam
                            int tmpi = beams[shocks[scount].beamid].shock->beamid; // beamID global of checked trigger beam
                            if (((short1 == short2 && long1 == long2) || (short1 == long2 && long1 == short2)) && i != tmpi) // found both command triggers then swap if its not the switching trigger
                            {
                                int tmpcmdkey = beams[shocks[scount].beamid].shock->trigger_cmdlong;
                                beams[shocks[scount].beamid].shock->trigger_cmdlong = beams[shocks[scount].beamid].shock->trigger_cmdshort;
                                beams[shocks[scount].beamid].shock->trigger_cmdshort = tmpcmdkey;
                                beam.shock->trigger_switch_state = beam.shock->trigger_boundary_t; //prevent trigger switching again before leaving boundaries or timeout
                                if (triggerdebug && beam.shock->last_debug_state != 3)
                                {
                                    LOG(" Trigger F-key commands switched. Switch BeamID " + TOSTRING(i)+ " switched commands of Trigger BeamID " + TOSTRING(beams[shocks[scount].beamid].shock->beamid) + " to cmdShort: F" + TOSTRING(beams[shocks[scount].beamid].shock->trigger_cmdshort) + ", cmdlong: F" + TOSTRING(beams[shocks[scount].beamid].shock->trigger_cmdlong));
                                    beam.shock->last_debug_state = 3;
                                }
                            }
                        }
                    }
                }
                else
                { // just a trigger, check high/low boundary and set action
                    if (cur_len_diff > beam.long_bound * beam.L) // trigger past long_bound
                    {
                        if (beam.shock->flags & SHOCK_FLAG_TRG_HOOK_UNLOCK)
                        {
                            if (update)
                            {
                                //autolock hooktoggle unlock
                                hookToggle(beam.shock->trigger_cmdlong, HOOK_UNLOCK, -1);
                            }
                        }
                        else if (beam.shock->flags & SHOCK_FLAG_TRG_HOOK_LOCK)
                        {
                            if (update)
                            {
                                //autolock hooktoggle lock
                                hookToggle(beam.shock->trigger_cmdlong, HOOK_LOCK, -1);
                            }
                        }
                        else if (beam.shock->flags & SHOCK_FLAG_TRG_ENGINE)
                        {
                            engineTriggerHelper(beam.shock->trigger_cmdshort, beam.shock->trigger_cmdlong, 1.0f);
                        }
                        else
                        {
                            //just a trigger
                            if (!commandkey[beam.shock->trigger_cmdlong].trigger_cmdkeyblock_state) // related cmdkey is not blocked
                            {
                                if (beam.shock->flags & SHOCK_FLAG_TRG_CONTINUOUS)
                                    commandkey[beam.shock->trigger_cmdshort].triggerInputValue = 1; // continuous trigger only operates on trigger_cmdshort
                                else
                                    commandkey[beam.shock->trigger_cmdlong].triggerInputValue = 1;
                                if (triggerdebug && beam.shock->last_debug_state != 4)
                                {
                                    LOG(" Trigger Longbound activated. Trigger BeamID " + TOSTRING(i) + " Triggered F" + TOSTRING(beam.shock->trigger_cmdlong));
                                    beam.shock->last_debug_state = 4;
                                }
                            }
                        }
                    }
                    else // trigger past short bound
                    {
                        if (beam.shock->flags & SHOCK_FLAG_TRG_HOOK_UNLOCK)
                        {
                            if (update)
                            {
                                //autolock hooktoggle unlock
                                hookToggle(beam.shock->trigger_cmdshort, HOOK_UNLOCK, -1);
                            }
                        }
                        else if (beam.shock->flags & SHOCK_FLAG_TRG_HOOK_LOCK)
                        {
                            if (update)
                            {
                                //autolock hooktoggle lock
                                hookToggle(beam.shock->trigger_cmdshort, HOOK_LOCK, -1);
                            }
                        }
                        else if (beam.shock->flags & SHOCK_FLAG_TRG_ENGINE)
                        {
                            bool triggerValue = !(beam.shock->flags & SHOCK_FLAG_TRG_CONTINUOUS); // 0 if trigger is continuous, 1 otherwise

                            engineTriggerHelper(beam.shock->trigger_cmdshort, beam.shock->trigger_cmdlong, triggerValue);
                        }
                        else
                        {
                            //just a trigger
                            if (!commandkey[beam.shock->trigger_cmdshort].trigger_cmdkeyblock_state) // related cmdkey is not blocked
                            {
                                if (beam.shock->flags & SHOCK_FLAG_TRG_CONTINUOUS)
                                    commandkey[beam.shock->trigger_cmdshort].triggerInputValue = 0; // continuous trigger only operates on trigger_cmdshort
                                else
                                    commandkey[beam.shock->trigger_cmdshort].triggerInputValue = 1;

                                if (triggerdebug && beam.shock->last_debug_state != 5)
                                {
                                    LOG(" Trigger Shortbound activated. Trigger BeamID " + TOSTRING(i) + " Triggered F" + TOSTRING(beam.shock->trigger_cmdshort));
                                    beam.shock->last_debug_state = 5;
                                }
                            }
                        }
                    }
                }
            }
            else // this is a trigger inside boundaries and its enabled
            {
                if (beam.shock->flags & SHOCK_FLAG_TRG_CONTINUOUS) // this is an enabled continuous trigger
                {
                    if (beam.long_bound - beam.short_bound > 0.0f)
                    {
                        float diffPercentage = cur_len_diff / beam.L;
                        float triggerValue = (diffPercentage - beam.short_bound) / (beam.long_bound - beam.short_bound);

                        triggerValue = std::max(0.0f, triggerValue);
                        triggerValue = std::min(triggerValue, 1.0f);

                        if (beam.shock->flags & SHOCK_FLAG_TRG_ENGINE) // this trigger controls an engine
                        {
                            engineTriggerHelper(beam.shock->trigger_cmdshort, beam.shock->trigger_cmdlong, triggerValue);
                        }
                        else
                        {
                            // normal trigger
                            commandkey[beam.shock->trigger_cmdshort].triggerInputValue = triggerValue;
                            commandkey[beam.shock->trigger_cmdlong].triggerInputValue = triggerValue;
                        }
                    }
                }
                else if (beam.shock->flags & SHOCK_FLAG_TRG_BLOCKER) // this is an enabled blocker and inside boundary
                {
                    for (int scount = i + 1; scount <= i + beam.shock->trigger_cmdlong; scount++) // (cycle blockerbeamID + 1) to (blockerbeamID + beams to release)
                    {
                        if (beams[scount].shock && (beams[scount].shock->flags & SHOCK_FLAG_ISTRIGGER)) // don't mess anything up if the user set the number too big
                        {
                            if (triggerdebug && beams[scount].shock->trigger_enabled && beam.shock->last_debug_state != 6)
                            {
                                LOG(" Trigger enabled. Blocker BeamID " + TOSTRING(i) + " disabled trigger " + TOSTRING(scount));
                                beam.shock->last_debug_state = 6;
                            }
                            beams[scount].shock->trigger_enabled = true; // enable the triggers
                        }
                    }
                }
                else if (beam.shock->flags & SHOCK_FLAG_TRG_BLOCKER_A) // this is an enabled reverse blocker and past boundary
                {
                    for (int scount = i + 1; scount <= i + beam.shock->trigger_cmdshort; scount++) // (cylce blockerbeamID +1) to (blockerbeamID + beams tob lock)
                    {
                        if (beams[scount].shock && (beams[scount].shock->flags & SHOCK_FLAG_ISTRIGGER)) // dont mess anything up if the user set the number too big
                        {
                            if (triggerdebug && !beams[scount].shock->trigger_enabled && beam.shock->last_debug_state != 10)
                            {
                                LOG(" Trigger disabled. Inverted Blocker BeamID " + TOSTRING(i) + " enabled trigger " + TOSTRING(scount));
                                beam.shock->last_debug_state = 10;
                            }
                            beams[scount].shock->trigger_enabled = false; // disable the trigger
                        }
                    }
                }
                else if ((beam.shock->flags & SHOCK_FLAG_TRG_CMD_SWITCH) && beam.shock->trigger_switch_state) // this is a switch that was activated and is back inside boundaries again
                {
                    beam.shock->trigger_switch_state = 0.0f; //trigger_switch reset
                    if (triggerdebug && beam.shock->last_debug_state != 7)
                    {
                        LOG(" Trigger switch reset. Switch BeamID " + TOSTRING(i));
                        beam.shock->last_debug_state = 7;
                    }
                }
                else if ((beam.shock->flags & SHOCK_FLAG_TRG_CMD_BLOCKER) && !commandkey[beam.shock->trigger_cmdshort].trigger_cmdkeyblock_state) // this cmdkeyblocker is inside boundaries and cmdkeystate is diabled
                {
                    commandkey[beam.shock->trigger_cmdshort].trigger_cmdkeyblock_state = true; // activate trigger blocking
                    if (triggerdebug && beam.shock->last_debug_state != 8)
                    {
                        LOG(" F-key trigger blocked. Blocker BeamID " + TOSTRING(i) + " Blocked F" + TOSTRING(beam.shock->trigger_cmdshort));
                        beam.shock->last_debug_state = 8;
                    }
                }
            }
        }
    }
    // save beam position for next simulation cycle
    beam.shock->lastpos = cur_len_diff;
}

void G1SoftbodyGraph::Calculate(std::vector<G1Node>& nodes, std::vector<G1Beam>& beams)
{
    // Temporary data storage
    std::vector< std::vector< int > > nodetonodeconnections;
    std::vector< std::vector< int > > nodebeamconnections;

    nodetonodeconnections.resize(nodes.size(), std::vector<int>());
    nodebeamconnections.resize(nodes.size(), std::vector<int>());

    for (size_t i = 0; i < beams.size(); i++)
    {
        if (beams[i].p1 != nullptr && beams[i].p2 != nullptr && beams[i].p1->pos >= 0 && beams[i].p2->pos >= 0)
        {
            nodetonodeconnections[beams[i].p1->pos].push_back(beams[i].p2->pos);
            nodebeamconnections[beams[i].p1->pos].push_back(i);
            nodetonodeconnections[beams[i].p2->pos].push_back(beams[i].p1->pos);
            nodebeamconnections[beams[i].p2->pos].push_back(i);
        }
    }

    // Final data storage
    m_node_info.resize(nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        m_node_info[i].beams_index = m_beams.size();
        m_node_info[i].beams_count = nodebeamconnections[i].size();
        for (int beam_id : nodebeamconnections[i])
        {
            m_beams.push_back(&beams[beam_id]);
        }

        m_node_info[i].nodes_index = m_nodes.size();
        m_node_info[i].nodes_count = nodetonodeconnections[i].size();
        for (int node_id : nodetonodeconnections[i])
        {
            m_nodes.push_back(&nodes[node_id]);
        }
    }
}

G1Node* G1SoftbodyGraph::GetNode(size_t index)
{
    return m_nodes[index];
}

G1Beam* G1SoftbodyGraph::GetBeam(size_t index)
{
    return m_beams[index];
}

size_t G1SoftbodyGraph::GetNumActiveNeighborBeams(int node_id)
{
    size_t count = 0;
    NodeConn info = m_node_info[node_id];
    size_t idx_max = info.nodes_index + info.nodes_count;
    for (size_t idx = info.nodes_index; idx < idx_max; ++idx)
    {
        G1Beam* b = m_beams[idx];
        if (!b->is_disabled && !b->is_shock1 && !b->is_shock2 && !b->is_rope && !b->is_support)
        {
            ++count;
        }
    }
    return count;
}


