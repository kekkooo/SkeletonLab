#pragma once

#include <skel/base.h>
#include <utilities/myGLutils.h>
#include <QList>
#ifdef OSX
#include <QGLViewer/qglviewer.h>
#endif
#ifdef WIN32
#include <qglviewer.h>
#endif

namespace Skel
{

	namespace Paint
	{

		enum DRAWMODE
		{
			BONES          = 0x0000000F,
			MAX_SPHERES    = 0x000000F0,
			FEATURE_POINTS = 0x00000F00,
			BONE_POINTS    = 0x0000F000,
			DEFAULT_COLOR  = 0x000F0000,
			BONE_COLOR     = 0x00F00000,
			PCA            = 0x0F000000,
			MAX_SPHERES_FP = 0xF0000000
		};

		static void paintGL(CurveSkeleton *cs, int mode, float metric)
		{
			if (cs == NULL)
			{
				std::cout << "no skeleton - exit from paint";
				return;
			}

			if (mode & BONES)
			{
				foreach (SkelPoint p, cs->points)
				{
					foreach (int neighIndex, p.neighbors)
					{
						SkelPoint neigh = cs->points[neighIndex];

                        MyGLutils::cylinder(p.coord, neigh.coord, metric * 0.5, metric * 0.5, ColorF::White() );
					}
				}
			}

			if (mode & FEATURE_POINTS)
			{
				foreach (SkelPoint p, cs->points)
				{
					if (p.isFeaturePoint() || (p.isRoot() && p.isDeleted()))
					{
						if (mode & BONE_COLOR)
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else
							{
								if (p.isLeaf())
								{
                                    MyGLutils::sphere(p.coord, metric * 2.f, p.color);
								}

								if (p.isArticulation())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
								}

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}
							}
						}
						else
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else
							{
								if (p.isLeaf())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Green());
								}                                

								if (p.isArticulation())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
								}

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}
							}
						}
					}
                    if (p.isSelected())     // quick and dirty fix
                    {
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                    }
				}
			}

			if (mode & BONE_POINTS)
			{
				foreach (SkelPoint p, cs->points)
				{
					//if (p.isB())
					if (p.isConnected())
					{
						if (mode & BONE_COLOR)
						{
                            //if is selected than paint it in yellow
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else if (p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else
							{
                                if( p.isJoint())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, p.color);
								}
							}
						}
						else
						{
                            if (p.isArticulation())
							{
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
							}							
                            else if( p.isJoint() )
							{
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Purple());
							}
						}
					}
					else
					{
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Purple());
                    }

                    if (p.isSelected())     // quick and dirty fix
                    {
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                    }
				}
			}

			if (mode & MAX_SPHERES_FP)
			{
                glEnable(GL_DEPTH_TEST);

				foreach (SkelPoint p, cs->points)
				{
					if (p.isFeaturePoint() || (p.isRoot() && p.isDeleted()))
					{
						if (mode & BONE_COLOR)
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else
							{
                                if( p.isBranchingNode() ){
                                    MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
                                }
                                else{
                                    MyGLutils::sphere(p.coord, p.radius, p.color);
                                }
							}
						}
						else
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else
							{
								if (p.isLeaf())
								{
									MyGLutils::sphere(p.coord, p.radius, ColorF::Green());
								}

								if (p.isArticulation())
								{
									MyGLutils::sphere(p.coord, p.radius, ColorF::Blue());
								}

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
								{
									MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
								}
							}
						}
					}
				}

//				glDisable(GL_DEPTH_TEST);
			}

			if (mode & MAX_SPHERES)
			{
                glEnable(GL_DEPTH_TEST);

				foreach (SkelPoint p, cs->points)
				{
					if (p.isConnected())
					{
						if (mode & BONE_COLOR)
						{
							//if is selected than paint it in yellow
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else if (p.isSelected())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::White());
							}
							else
							{
                                if (p.isJoint())
								{
									MyGLutils::sphere(p.coord, p.radius, p.color);
								}
							}
						}
						else
						{
                            if (p.isRoot() && p.isDeleted())
							{
								MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
							}
                            if( p.isJoint() && !p.isArticulation( ))
							{
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Purple());
							}
                            if( p.isArticulation())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Blue());
                            }
						}
					}
					else
					{
                        MyGLutils::sphere(p.coord, p.radius, ColorF::Purple());
					}
                    if( p.isSelected())
                    {
                        MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
                    }
				}

//				glDisable(GL_DEPTH_TEST);
			}

			glEnable(GL_DEPTH_TEST);
		}



        static void paintGLWithNames(CurveSkeleton *cs, int mode, float metric)
        {
            if (cs == NULL)
            {
                std::cout << "no skeleton - exit from paint";
                return;
            }

            if (mode & FEATURE_POINTS)
            {
                foreach (SkelPoint p, cs->points)
                {
                    if (p.isFeaturePoint() || (p.isRoot() && p.isDeleted()))
                    {
                        glPushMatrix();
                        glPushName(p.id);

                        if (mode & BONE_COLOR)
                        {
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else if(p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else
                            {
                                if (p.isLeaf())
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, p.color);
                                }

                                if (p.isArticulation())
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
                                }

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
                                }
                            }
                        }
                        else
                        {
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else if(p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else
                            {
                                if (p.isLeaf())
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Green());
                                }

                                if (p.isArticulation())
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
                                }

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
                                }
                            }
                        }
                    }
                    if (p.isSelected())     // quick and dirty fix
                    {
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                    }
                    glPopName();
                    glPopMatrix();
                }
            }

            if (mode & BONE_POINTS)
            {
                foreach (SkelPoint p, cs->points)
                {
                    glPushMatrix();
                    glPushName(p.id);
                    //if (p.isB())
                    if (p.isConnected())
                    {
                        if (mode & BONE_COLOR)
                        {
                            //if is selected than paint it in yellow
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else if (p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
                            }
                            else
                            {
                                if( p.isJoint())
                                {
                                    MyGLutils::sphere(p.coord, metric * 2.f, p.color);
                                }
                            }
                        }
                        else
                        {
                            if (p.isArticulation())
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Blue());
                            }
                            else if( p.isJoint() )
                            {
                                MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Purple());
                            }
                        }
                    }
                    else
                    {
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Purple());
                    }

                    if (p.isSelected())     // quick and dirty fix
                    {
                        MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
                    }
                    glPopName();
                    glPopMatrix();
                }
            }

            if (mode & MAX_SPHERES_FP)
            {
                glEnable(GL_DEPTH_TEST);

                foreach (SkelPoint p, cs->points)
                {
                    if (p.isFeaturePoint() || (p.isRoot() && p.isDeleted()))
                    {
                        if (mode & BONE_COLOR)
                        {
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else if(p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else
                            {
                                if( p.isBranchingNode() ){
                                    MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
                                }
                                else{
                                    MyGLutils::sphere(p.coord, p.radius, p.color);
                                }
                            }
                        }
                        else
                        {
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else if(p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else
                            {
                                if (p.isLeaf())
                                {
                                    MyGLutils::sphere(p.coord, p.radius, ColorF::Green());
                                }

                                if (p.isArticulation())
                                {
                                    MyGLutils::sphere(p.coord, p.radius, ColorF::Blue());
                                }

                                if (( p.isRoot() && p.isDeleted()) || p.isBranchingNode( ))
                                {
                                    MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
                                }
                            }
                        }
                    }
                }

        //				glDisable(GL_DEPTH_TEST);
            }

            if (mode & MAX_SPHERES)
            {
                glEnable(GL_DEPTH_TEST);

                foreach (SkelPoint p, cs->points)
                {
                    if (p.isConnected())
                    {
                        if (mode & BONE_COLOR)
                        {
                            //if is selected than paint it in yellow
                            if (p.isTransferOnIt())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else if (p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
                            }
                            else if (p.isSelectedForCopy())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::White());
                            }
                            else
                            {
                                if (p.isJoint())
                                {
                                    MyGLutils::sphere(p.coord, p.radius, p.color);
                                }
                            }
                        }
                        else
                        {
                            if (p.isRoot() && p.isDeleted())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Red());
                            }
                            if( p.isJoint() && !p.isArticulation( ))
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Purple());
                            }
                            if( p.isArticulation())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Blue());
                            }
                            if( p.isSelected())
                            {
                                MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
                            }
                        }
                    }
                    else
                    {
                        MyGLutils::sphere(p.coord, p.radius, ColorF::Purple());
                    }
                }

        //				glDisable(GL_DEPTH_TEST);
            }

            glEnable(GL_DEPTH_TEST);
        }

		static void paintGLWithNamesInBoneMode(CurveSkeleton *cs, int mode, float metric)
		{
			if (cs == NULL)
			{
				std::cout << "no skeleton - exit from paint";
				return;
			}

			// enable it in order to see the skeleton inside a mesh.
			//	glDisable(GL_DEPTH_TEST);

			if (mode & FEATURE_POINTS)
			{
				foreach (SkelPoint p, cs->points)
				{
					if (p.isFeaturePoint())
					{
						glPushMatrix();
						glPushName(p.boneID);

						if (mode & BONE_COLOR)
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else
							{
								if (p.isLeaf())
								{
									int index = p.neighbors.first();

									MyGLutils::sphere(p.coord, metric * 2.f, cs->points[index].color);
								}

								if (p.isBranchingNode())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}

								if (p.isArticulation())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}
							}
						}
						else
						{
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else if(p.isSelected())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else
							{
								if (p.isLeaf())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Green());
								}

								if (p.isBranchingNode())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}

								if (p.isArticulation())
								{
									MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Red());
								}
							}
						}

						glPopName();
						glPopMatrix();
					}
				}
			}

			if (mode & BONE_POINTS)
			{
				foreach (SkelPoint p, cs->points)
				{
					glPushMatrix();
					glPushName(p.boneID);

					//if (p.isB())
					if (p.isConnected())
					{
						if (mode & BONE_COLOR)
						{
							//if is selected than paint it in yellow
							if (p.isTransferOnIt())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else if (p.isSelected())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::Yellow());
							}
							else if (p.isSelectedForCopy())
							{
								MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
							}
							else
							{
								MyGLutils::sphere(p.coord, metric * 2.f, p.color);
							}
						}
						else
						{
							MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
						}
					}
					else
					{
						MyGLutils::sphere(p.coord, metric * 2.f, ColorF::White());
					}

					glPopName();
					glPopMatrix();
				}
			}

			if (mode & MAX_SPHERES_FP)
			{
				glEnable(GL_DEPTH_TEST);

				foreach (SkelPoint p, cs->points)
				{
					if (!p.isFeaturePoint())
					{
						continue;
					}

					glPushMatrix();
					glPushName(p.boneID);

					if (p.isTransferOnIt())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::White());
					}
					else if(p.isSelected())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
					}
					else if (p.isSelectedForCopy())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::White());
					}
					else
					{
						if (mode & BONE_COLOR)
						{
							MyGLutils::sphere(p.coord, p.radius, p.color);
						}
						else
						{
							MyGLutils::sphere(p.coord, p.radius, ColorF::White());
						}
					}

					glPopName();
					glPopMatrix();
				}

//				glDisable(GL_DEPTH_TEST);
			}

			if (mode & MAX_SPHERES)
			{
				glEnable(GL_DEPTH_TEST);

				foreach (SkelPoint p, cs->points)
				{
					if(p.isFeaturePoint())
					{
						continue;
					}

					glPushMatrix();
					glPushName(p.boneID);

					if (p.isTransferOnIt())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::White());
					}
					else if(p.isSelected())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::Yellow());
					}
					else if (p.isSelectedForCopy())
					{
						MyGLutils::sphere(p.coord, p.radius, ColorF::White());
					}
					else
					{
						if (mode & BONE_COLOR)
						{
							MyGLutils::sphere(p.coord, p.radius, p.color);
						}
						else
						{
							MyGLutils::sphere(p.coord, p.radius, ColorF::White());
						}
					}

					glPopName();
					glPopMatrix();
				}

//				glDisable(GL_DEPTH_TEST);
			}

			glEnable(GL_DEPTH_TEST);
		}
	}
}
